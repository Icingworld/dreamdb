#include "dreamdb/planner/logical/logical_planner.h"

#include <stdexcept>
#include <unordered_set>
#include <algorithm>

#include "dreamdb/binder/bound/statement/statement.h"
#include "dreamdb/binder/bound/statement/select.h"
#include "dreamdb/binder/bound/statement/delete.h"
#include "dreamdb/binder/bound/statement/update.h"
#include "dreamdb/binder/bound/expression/column_reference.h"
#include "dreamdb/binder/bound/expression/constant.h"
#include "dreamdb/binder/bound/expression/binary.h"
#include "dreamdb/binder/bound/expression/unary.h"
#include "dreamdb/binder/bound/expression/function_call.h"
#include "dreamdb/binder/bound/expression/in.h"
#include "dreamdb/binder/bound/expression/like.h"
#include "dreamdb/binder/bound/expression/between.h"
#include "dreamdb/planner/logical/logical_operator.h"
#include "dreamdb/planner/logical/operator/logical_scan.h"
#include "dreamdb/planner/logical/operator/logical_filter.h"
#include "dreamdb/planner/logical/operator/logical_project.h"
#include "dreamdb/planner/logical/operator/logical_aggregate.h"
#include "dreamdb/planner/logical/operator/logical_sort.h"
#include "dreamdb/planner/logical/operator/logical_limit_offset.h"
#include "dreamdb/planner/logical/operator/logical_delete.h"
#include "dreamdb/planner/logical/operator/logical_update.h"
#include "dreamdb/common/ids.h"

namespace dreamdb::planner::logical
{

void BoundColumnCollector::visit(
    const dreamdb::binder::bound::BoundColumnReferenceExpression & column_reference_expression
)
{
    // 收集列引用中的列 ID
    column_ids.push_back(column_reference_expression.column_id());
}

void BoundColumnCollector::visit(
    const dreamdb::binder::bound::BoundConstantExpression & constant_expression
)
{
    // 常量表达式不包含列引用，无需处理
    (void)constant_expression;
}

void BoundColumnCollector::visit(
    const dreamdb::binder::bound::BoundFunctionCallExpression & function_call_expression
)
{
    // 递归访问所有参数表达式
    for (std::size_t i = 0; i < function_call_expression.argument_count(); ++i) {
        function_call_expression.argument_at(i).accept(*this);
    }
}

void BoundColumnCollector::visit(
    const dreamdb::binder::bound::BoundInExpression & in_expression
)
{
    // 递归访问左侧表达式
    in_expression.left().accept(*this);

    // 递归访问所有值表达式
    for (std::size_t i = 0; i < in_expression.value_count(); ++i) {
        in_expression.value_at(i).accept(*this);
    }
}

void BoundColumnCollector::visit(
    const dreamdb::binder::bound::BoundBetweenExpression & between_expression
)
{
    // 递归访问左侧表达式
    between_expression.left().accept(*this);

    // 递归访问起始值表达式
    between_expression.start().accept(*this);

    // 递归访问结束值表达式
    between_expression.end().accept(*this);
}

void BoundColumnCollector::visit(
    const dreamdb::binder::bound::BoundLikeExpression & like_expression
)
{
    // 递归访问左侧表达式
    like_expression.left().accept(*this);

    // 递归访问模式表达式
    like_expression.pattern().accept(*this);
}

void BoundColumnCollector::visit(
    const dreamdb::binder::bound::BoundUnaryExpression & unary_expression
)
{
    // 递归访问操作数表达式
    unary_expression.operand().accept(*this);
}

void BoundColumnCollector::visit(
    const dreamdb::binder::bound::BoundBinaryExpression & binary_expression
)
{
    // 递归访问左操作数表达式
    binary_expression.left().accept(*this);

    // 递归访问右操作数表达式
    binary_expression.right().accept(*this);
}

std::vector<dreamdb::common::column_id_t> LogicalPlanner::collect_column_ids(
    const dreamdb::binder::bound::BoundExpression & bound_expression
)
{
    BoundColumnCollector collector;
    bound_expression.accept(collector);

    // 去重处理：使用 unordered_set 去重，然后转回 vector
    std::unordered_set<dreamdb::common::column_id_t> unique_column_ids(
        collector.column_ids.begin(),
        collector.column_ids.end()
    );

    std::vector<dreamdb::common::column_id_t> result(
        unique_column_ids.begin(),
        unique_column_ids.end()
    );

    // 排序以便后续处理
    std::sort(result.begin(), result.end());

    return result;
}

std::unique_ptr<LogicalOperator> LogicalPlanner::plan(
    const dreamdb::binder::bound::BoundStatement & bound_statement
) const
{
    // 根据绑定后的语句的类型，选择对应的计划
    switch (bound_statement.statement_type()) {
        case dreamdb::binder::bound::BoundStatementType::Select:
            return plan_select(
                static_cast<const dreamdb::binder::bound::BoundSelectStatement &>(bound_statement)
            );
        case dreamdb::binder::bound::BoundStatementType::Delete:
            return plan_delete(
                static_cast<const dreamdb::binder::bound::BoundDeleteStatement &>(bound_statement)
            );
        case dreamdb::binder::bound::BoundStatementType::Update:
            return plan_update(
                static_cast<const dreamdb::binder::bound::BoundUpdateStatement &>(bound_statement)
            );
        default:
            throw std::runtime_error(
                "Unsupported bound statement type: " +
                std::to_string(static_cast<std::uint8_t>(bound_statement.statement_type()))
            );
    }
}

std::unique_ptr<LogicalOperator> LogicalPlanner::plan_select(
    const dreamdb::binder::bound::BoundSelectStatement & select_statement
) const
{
    // 获取集合 ID
    const auto collection_id = select_statement.collection_id();

    // 收集所有需要的列 ID
    std::unordered_set<dreamdb::common::column_id_t> required_columns_set;

    // 从 SELECT 列表中收集
    for (std::size_t i = 0; i < select_statement.select_item_count(); ++i) {
        const auto & select_item = select_statement.select_item_at(i);
        const auto columns = collect_column_ids(*select_item.expr);
        required_columns_set.insert(columns.begin(), columns.end());
    }

    // 从 WHERE 子句中收集
    if (select_statement.has_where()) {
        const auto where_columns = collect_column_ids(select_statement.where_ref());
        required_columns_set.insert(where_columns.begin(), where_columns.end());
    }

    // 从 GROUP BY 中收集
    for (std::size_t i = 0; i < select_statement.group_by_count(); ++i) {
        const auto * group_by_expr = select_statement.group_by_at(i);
        if (group_by_expr != nullptr) {
            const auto group_by_columns = collect_column_ids(*group_by_expr);
            required_columns_set.insert(group_by_columns.begin(), group_by_columns.end());
        }
    }

    // 从 HAVING 子句中收集
    if (select_statement.has_having()) {
        const auto having_columns = collect_column_ids(select_statement.having_ref());
        required_columns_set.insert(having_columns.begin(), having_columns.end());
    }

    // 从 ORDER BY 中收集
    for (std::size_t i = 0; i < select_statement.order_by_count(); ++i) {
        const auto & order_item = select_statement.order_by_at(i);
        const auto order_columns = collect_column_ids(*order_item.expr);
        required_columns_set.insert(order_columns.begin(), order_columns.end());
    }

    // 转换为有序的列 ID 列表
    std::vector<dreamdb::common::column_id_t> required_columns_ids(
        required_columns_set.begin(),
        required_columns_set.end()
    );
    std::sort(required_columns_ids.begin(), required_columns_ids.end());

    // 1. 创建 Scan 算子（叶子节点）
    std::unique_ptr<LogicalOperator> root_operator = std::make_unique<LogicalScan>(
        collection_id, std::move(required_columns_ids)
    );

    // 2. 如果有 WHERE 子句，添加 Filter 节点
    if (select_statement.has_where()) {
        auto predicate = select_statement.where_ref().clone();
        auto filter_operator = std::make_unique<LogicalFilter>(std::move(predicate));
        filter_operator->add_child(std::move(root_operator));
        root_operator = std::move(filter_operator);
    }

    // 3. 检查是否有聚合函数
    bool has_aggregate = false;
    for (std::size_t i = 0; i < select_statement.select_item_count(); ++i) {
        const auto & select_item = select_statement.select_item_at(i);
        if (select_item.expr->expression_type() ==
            dreamdb::binder::bound::BoundExpressionType::FunctionCall) {
            const auto & func_expr = static_cast<const dreamdb::binder::bound::BoundFunctionCallExpression &>(
                *select_item.expr
            );
            if (func_expr.is_aggregate()) {
                has_aggregate = true;
                break;
            }
        }
    }

    // 4. 如果有 GROUP BY 或聚合函数，添加 Aggregate 节点
    if (select_statement.group_by_count() > 0 || has_aggregate) {
        // 复制 GROUP BY 表达式
        std::vector<std::unique_ptr<dreamdb::binder::bound::BoundExpression>> group_by;
        group_by.reserve(select_statement.group_by_count());
        for (std::size_t i = 0; i < select_statement.group_by_count(); ++i) {
            const auto * group_by_expr = select_statement.group_by_at(i);
            if (group_by_expr != nullptr) {
                group_by.push_back(group_by_expr->clone());
            }
        }

        // 转换 SELECT 列表为聚合项
        std::vector<LogicalAggregateItem> aggregate_items;
        aggregate_items.reserve(select_statement.select_item_count());
        for (std::size_t i = 0; i < select_statement.select_item_count(); ++i) {
            const auto & select_item = select_statement.select_item_at(i);
            aggregate_items.push_back({
                select_item.expr->clone(),
                select_item.alias.empty() ? std::nullopt : std::make_optional(select_item.alias)
            });
        }

        auto aggregate_operator = std::make_unique<LogicalAggregate>(
            std::move(group_by),
            std::move(aggregate_items)
        );
        aggregate_operator->add_child(std::move(root_operator));
        root_operator = std::move(aggregate_operator);
    }

    // 5. 如果有 HAVING 子句，添加 Filter 节点（在 Aggregate 之后）
    if (select_statement.has_having()) {
        auto predicate = select_statement.having_ref().clone();
        auto having_filter = std::make_unique<LogicalFilter>(std::move(predicate));
        having_filter->add_child(std::move(root_operator));
        root_operator = std::move(having_filter);
    }

    // 6. 如果没有聚合，添加 Project 节点（如果有聚合，Project 已经在 Aggregate 中处理了）
    if (select_statement.group_by_count() == 0 && !has_aggregate) {
        std::vector<LogicalProjectItem> project_items;
        project_items.reserve(select_statement.select_item_count());
        for (std::size_t i = 0; i < select_statement.select_item_count(); ++i) {
            const auto & select_item = select_statement.select_item_at(i);
            project_items.push_back({
                select_item.expr->clone(),
                select_item.alias.empty() ? std::nullopt : std::make_optional(select_item.alias)
            });
        }

        auto project_operator = std::make_unique<LogicalProject>(std::move(project_items));
        project_operator->add_child(std::move(root_operator));
        root_operator = std::move(project_operator);
    }

    // 7. 如果有 ORDER BY，添加 Sort 节点
    if (select_statement.order_by_count() > 0) {
        std::vector<LogicalSortItem> sort_items;
        sort_items.reserve(select_statement.order_by_count());
        for (std::size_t i = 0; i < select_statement.order_by_count(); ++i) {
            const auto & order_item = select_statement.order_by_at(i);
            sort_items.push_back({
                order_item.expr->clone(),
                order_item.direction
            });
        }

        auto sort_operator = std::make_unique<LogicalSort>(std::move(sort_items));
        sort_operator->add_child(std::move(root_operator));
        root_operator = std::move(sort_operator);
    }

    // 8. 如果有 LIMIT 或 OFFSET，添加 LimitOffset 节点
    if (select_statement.has_limit() || select_statement.has_offset()) {
        std::optional<std::size_t> limit = select_statement.has_limit() ?
            std::make_optional(select_statement.limit()) : std::nullopt;
        std::optional<std::size_t> offset = select_statement.has_offset() ?
            std::make_optional(select_statement.offset()) : std::nullopt;

        auto limit_offset_operator = std::make_unique<LogicalLimitOffset>(
            limit,
            offset
        );
        limit_offset_operator->add_child(std::move(root_operator));
        root_operator = std::move(limit_offset_operator);
    }

    // 返回 Root 算子
    return root_operator;
}

std::unique_ptr<LogicalOperator> LogicalPlanner::plan_delete(
    const dreamdb::binder::bound::BoundDeleteStatement & delete_statement
) const
{
    // 获取集合 ID
    const auto collection_id = delete_statement.collection_id();

    // 准备 Scan 算子所需的列数组
    std::vector<dreamdb::common::column_id_t> required_columns_ids;

    // 获取 WHERE 子句
    // 如果不存在 WHERE 子句，则表明不需要扫描任何列，删除所有数据
    if (delete_statement.has_where()) {
        required_columns_ids = LogicalPlanner::collect_column_ids(delete_statement.where_ref());
    }

    // 创建 Scan 算子
    std::unique_ptr<LogicalOperator> root_operator = std::make_unique<LogicalScan>(
        collection_id, std::move(required_columns_ids)
    );

    // 创建 Filter 算子
    if (delete_statement.has_where()) {
        // 克隆 WHERE 子句
        auto predicate = delete_statement.where_ref().clone();

        // 创建 Filter 算子
        auto filter_operator = std::make_unique<LogicalFilter>(std::move(predicate));

        // 将 Scan 算子添加到 Filter 算子中
        filter_operator->add_child(std::move(root_operator));

        // 移动 Root 算子
        root_operator = std::move(filter_operator);
    }

    // 创建 Delete 算子
    auto delete_operator = std::make_unique<LogicalDelete>(collection_id);

    // 将 Filter 算子添加到 Delete 算子中
    delete_operator->add_child(std::move(root_operator));

    // 移动 Root 算子
    root_operator = std::move(delete_operator);

    // 返回 Root 算子
    return root_operator;
}

std::unique_ptr<LogicalOperator> LogicalPlanner::plan_update(
    const dreamdb::binder::bound::BoundUpdateStatement & update_statement
) const
{
    // 获取集合 ID
    const auto collection_id = update_statement.collection_id();

    // 准备 Scan 算子所需的列数组
    std::unordered_set<dreamdb::common::column_id_t> required_columns_set;

    // 从 WHERE 子句中收集列引用
    if (update_statement.has_where()) {
        const auto where_columns = collect_column_ids(update_statement.where_ref());
        required_columns_set.insert(where_columns.begin(), where_columns.end());
    }

    // 从更新项中收集列引用
    std::vector<LogicalUpdateItem> logical_update_items;
    logical_update_items.reserve(update_statement.update_item_count());

    for (std::size_t i = 0; i < update_statement.update_item_count(); ++i) {
        const auto & bound_item = update_statement.update_item_at(i);

        // 提取列 ID（column_reference 应该是 BoundColumnReferenceExpression）
        if (bound_item.column_reference->expression_type() !=
            dreamdb::binder::bound::BoundExpressionType::ColumnReference) {
            throw std::runtime_error("UPDATE item column_reference must be a BoundColumnReferenceExpression");
        }

        const auto & column_ref = static_cast<const dreamdb::binder::bound::BoundColumnReferenceExpression &>(
            *bound_item.column_reference
        );
        const auto column_id = column_ref.column_id();

        // 收集该列 ID（需要扫描该列以便更新）
        required_columns_set.insert(column_id);

        // 收集值表达式中的列引用
        const auto value_columns = collect_column_ids(*bound_item.value);
        required_columns_set.insert(value_columns.begin(), value_columns.end());

        // 创建 LogicalUpdateItem
        logical_update_items.push_back({
            column_id,
            bound_item.value->clone()  // 克隆值表达式
        });
    }

    // 转换为有序的列 ID 列表
    std::vector<dreamdb::common::column_id_t> required_columns_ids(
        required_columns_set.begin(),
        required_columns_set.end()
    );
    std::sort(required_columns_ids.begin(), required_columns_ids.end());

    // 创建 Scan 算子
    std::unique_ptr<LogicalOperator> root_operator = std::make_unique<LogicalScan>(
        collection_id, std::move(required_columns_ids)
    );

    // 创建 Filter 算子（如果有 WHERE 子句）
    if (update_statement.has_where()) {
        // 克隆 WHERE 子句
        auto predicate = update_statement.where_ref().clone();

        // 创建 Filter 算子
        auto filter_operator = std::make_unique<LogicalFilter>(std::move(predicate));

        // 将 Scan 算子添加到 Filter 算子中
        filter_operator->add_child(std::move(root_operator));

        // 移动 Root 算子
        root_operator = std::move(filter_operator);
    }

    // 创建 Update 算子
    auto update_operator = std::make_unique<LogicalUpdate>(
        collection_id,
        std::move(logical_update_items)
    );

    // 将 Filter 算子（或 Scan 算子）添加到 Update 算子中
    update_operator->add_child(std::move(root_operator));

    // 返回 Root 算子
    return update_operator;
}

} // namespace dreamdb::planner::logical
