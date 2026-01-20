#include "dreamdb/planner/logical_planner/logical_planner.h"

#include <stdexcept>
#include <memory>
#include <vector>
#include <unordered_set>
#include <algorithm>

#include "dreamdb/planner/logical_planner/update/logical_update_plan_node.h"
#include "dreamdb/planner/logical_planner/delete/logical_delete_plan_node.h"
#include "dreamdb/planner/logical_planner/select/logical_scan_node.h"
#include "dreamdb/planner/logical_planner/select/logical_filter_node.h"
#include "dreamdb/planner/logical_planner/select/logical_project_node.h"
#include "dreamdb/planner/logical_planner/select/logical_aggregate_node.h"
#include "dreamdb/planner/logical_planner/select/logical_sort_node.h"
#include "dreamdb/planner/logical_planner/select/logical_limit_offset_node.h"
#include "dreamdb/expression/column_reference.h"
#include "dreamdb/expression/constant.h"
#include "dreamdb/expression/binary.h"
#include "dreamdb/expression/unary.h"
#include "dreamdb/expression/function.h"
#include "dreamdb/expression/in.h"
#include "dreamdb/expression/like.h"
#include "dreamdb/expression/between.h"
#include "dreamdb/expression/null.h"

namespace dreamdb
{

namespace
{

/**
 * @brief 复制表达式（辅助函数）
 * @details 递归复制表达式树，支持所有表达式类型
 * @param expr 要复制的表达式
 * @return 复制后的表达式，如果输入为 nullptr 则返回 nullptr
 */
std::unique_ptr<Expression> copy_expression(const Expression * expr)
{
    if (!expr) {
        return nullptr;
    }

    switch (expr->get_type()) {
        case ExpressionType::EXPRESSION_COLUMN_REFERENCE: {
            const auto * col_ref = static_cast<const ColumnReferenceExpression *>(expr);
            return std::make_unique<ColumnReferenceExpression>(col_ref->get_field_index());
        }

        case ExpressionType::EXPRESSION_CONSTANT: {
            const auto * const_expr = static_cast<const ConstantExpression *>(expr);
            return std::make_unique<ConstantExpression>(const_expr->get_field_value());
        }

        case ExpressionType::EXPRESSION_BINARY: {
            const auto * binary_expr = static_cast<const BinaryExpression *>(expr);
            auto left = copy_expression(&binary_expr->get_left());
            auto right = copy_expression(&binary_expr->get_right());
            return std::make_unique<BinaryExpression>(
                binary_expr->get_operator_type(),
                std::move(left),
                std::move(right)
            );
        }

        case ExpressionType::EXPRESSION_UNARY: {
            const auto * unary_expr = static_cast<const UnaryExpression *>(expr);
            auto operand = copy_expression(&unary_expr->get_operand());
            return std::make_unique<UnaryExpression>(
                unary_expr->get_operator_type(),
                std::move(operand)
            );
        }

        case ExpressionType::EXPRESSION_FUNCTION: {
            const auto * func_expr = static_cast<const FunctionExpression *>(expr);
            std::vector<std::unique_ptr<Expression>> arguments;
            arguments.reserve(func_expr->get_arguments().size());
            for (const auto & arg : func_expr->get_arguments()) {
                arguments.push_back(copy_expression(arg.get()));
            }
            return std::make_unique<FunctionExpression>(
                func_expr->get_function_name(),
                std::move(arguments),
                func_expr->is_aggregate()
            );
        }

        case ExpressionType::EXPRESSION_IN: {
            const auto * in_expr = static_cast<const InExpression *>(expr);
            auto value = copy_expression(&in_expr->get_value());
            std::vector<std::unique_ptr<Expression>> list;
            list.reserve(in_expr->get_list().size());
            for (const auto & item : in_expr->get_list()) {
                list.push_back(copy_expression(item.get()));
            }
            return std::make_unique<InExpression>(
                std::move(value),
                std::move(list),
                in_expr->is_negated()
            );
        }

        case ExpressionType::EXPRESSION_LIKE: {
            const auto * like_expr = static_cast<const LikeExpression *>(expr);
            auto value = copy_expression(&like_expr->get_value());
            auto pattern = copy_expression(&like_expr->get_pattern());
            return std::make_unique<LikeExpression>(
                std::move(value),
                std::move(pattern),
                like_expr->is_negated()
            );
        }

        case ExpressionType::EXPRESSION_BETWEEN: {
            const auto * between_expr = static_cast<const BetweenExpression *>(expr);
            auto value = copy_expression(&between_expr->get_value());
            auto min = copy_expression(&between_expr->get_min());
            auto max = copy_expression(&between_expr->get_max());
            return std::make_unique<BetweenExpression>(
                std::move(value),
                std::move(min),
                std::move(max),
                between_expr->is_negated()
            );
        }

        case ExpressionType::EXPRESSION_NULL: {
            const auto * null_expr = static_cast<const NullExpression *>(expr);
            auto value = copy_expression(&null_expr->get_value());
            return std::make_unique<NullExpression>(
                std::move(value),
                null_expr->is_negated()
            );
        }

        default:
            throw std::runtime_error("Unknown expression type: " + 
                std::to_string(static_cast<int>(expr->get_type())));
    }
}

/**
 * @brief 从表达式中收集所有列引用索引
 * @param expr 表达式
 * @param field_indexes 用于存储字段索引的集合
 */
void collect_column_references(const Expression * expr, std::unordered_set<std::size_t> & field_indexes)
{
    if (!expr) {
        return;
    }

    switch (expr->get_type()) {
        case ExpressionType::EXPRESSION_COLUMN_REFERENCE: {
            const auto * col_ref = static_cast<const ColumnReferenceExpression *>(expr);
            field_indexes.insert(col_ref->get_field_index());
            break;
        }

        case ExpressionType::EXPRESSION_BINARY: {
            const auto * binary_expr = static_cast<const BinaryExpression *>(expr);
            collect_column_references(&binary_expr->get_left(), field_indexes);
            collect_column_references(&binary_expr->get_right(), field_indexes);
            break;
        }

        case ExpressionType::EXPRESSION_UNARY: {
            const auto * unary_expr = static_cast<const UnaryExpression *>(expr);
            collect_column_references(&unary_expr->get_operand(), field_indexes);
            break;
        }

        case ExpressionType::EXPRESSION_FUNCTION: {
            const auto * func_expr = static_cast<const FunctionExpression *>(expr);
            for (const auto & arg : func_expr->get_arguments()) {
                collect_column_references(arg.get(), field_indexes);
            }
            break;
        }

        case ExpressionType::EXPRESSION_IN: {
            const auto * in_expr = static_cast<const InExpression *>(expr);
            collect_column_references(&in_expr->get_value(), field_indexes);
            for (const auto & item : in_expr->get_list()) {
                collect_column_references(item.get(), field_indexes);
            }
            break;
        }

        case ExpressionType::EXPRESSION_LIKE: {
            const auto * like_expr = static_cast<const LikeExpression *>(expr);
            collect_column_references(&like_expr->get_value(), field_indexes);
            collect_column_references(&like_expr->get_pattern(), field_indexes);
            break;
        }

        case ExpressionType::EXPRESSION_BETWEEN: {
            const auto * between_expr = static_cast<const BetweenExpression *>(expr);
            collect_column_references(&between_expr->get_value(), field_indexes);
            collect_column_references(&between_expr->get_min(), field_indexes);
            collect_column_references(&between_expr->get_max(), field_indexes);
            break;
        }

        case ExpressionType::EXPRESSION_NULL: {
            const auto * null_expr = static_cast<const NullExpression *>(expr);
            collect_column_references(&null_expr->get_value(), field_indexes);
            break;
        }

        case ExpressionType::EXPRESSION_CONSTANT:
            // 常量表达式不包含列引用
            break;

        default:
            // 忽略未知类型
            break;
    }
}

} // anonymous namespace

std::unique_ptr<LogicalPlanNode> LogicalPlanner::plan(const BoundStatement & bound_statement) const
{
    // 根据绑定后的语句的类型，选择对应的计划
    switch (bound_statement.get_type()) {
        case BoundStatementType::BINDER_BOUND_SELECT_STATEMENT:
            return plan_select(static_cast<const BoundSelectStatement &>(bound_statement));
        case BoundStatementType::BINDER_BOUND_DELETE_STATEMENT:
            return plan_delete(static_cast<const BoundDeleteStatement &>(bound_statement));
        case BoundStatementType::BINDER_BOUND_UPDATE_STATEMENT:
            return plan_update(static_cast<const BoundUpdateStatement &>(bound_statement));
        case BoundStatementType::BINDER_BOUND_INSERT_STATEMENT:
            // INSERT 语句不需要经过 Planner，直接由 Executor 执行
            throw std::runtime_error("INSERT statement should not be planned, it should be executed directly");
        default:
            throw std::runtime_error("Unsupported bound statement type: " + std::to_string(static_cast<int>(bound_statement.get_type())));
    }
}

std::unique_ptr<LogicalPlanNode> LogicalPlanner::plan_select(const BoundSelectStatement & bound_select_statement) const
{
    // 收集所有需要的字段索引
    std::unordered_set<std::size_t> field_index_set;

    // 从 SELECT 列表中收集
    for (const auto & select_item : bound_select_statement.select_items) {
        collect_column_references(select_item.expr.get(), field_index_set);
    }

    // 从 WHERE 子句中收集
    if (bound_select_statement.where) {
        collect_column_references(bound_select_statement.where.get(), field_index_set);
    }

    // 从 GROUP BY 中收集
    for (const auto & group_expr : bound_select_statement.group_by) {
        collect_column_references(group_expr.get(), field_index_set);
    }

    // 从 HAVING 子句中收集
    if (bound_select_statement.having) {
        collect_column_references(bound_select_statement.having.get(), field_index_set);
    }

    // 从 ORDER BY 中收集
    for (const auto & order_item : bound_select_statement.order_by) {
        collect_column_references(order_item.expr.get(), field_index_set);
    }

    // 转换为有序的字段索引列表
    std::vector<std::size_t> field_indexes(field_index_set.begin(), field_index_set.end());
    std::sort(field_indexes.begin(), field_indexes.end());

    // 1. 创建 Scan 节点（叶子节点）
    std::unique_ptr<LogicalPlanNode> root = std::make_unique<LogicalScanNode>(
        bound_select_statement.collection_id,
        std::move(field_indexes)
    );

    // 2. 如果有 WHERE 子句，添加 Filter 节点
    if (bound_select_statement.where) {
        auto filter = std::make_unique<LogicalFilterNode>(
            copy_expression(bound_select_statement.where.get())
        );
        filter->get_mutable_children().push_back(std::move(root));
        root = std::move(filter);
    }

    // 3. 如果有 GROUP BY 或聚合函数，添加 Aggregate 节点
    // 检查是否有聚合函数（通过检查 SELECT 列表中的函数表达式是否为聚合函数）
    bool has_aggregate = false;
    for (const auto & select_item : bound_select_statement.select_items) {
        if (select_item.expr && select_item.expr->get_type() == ExpressionType::EXPRESSION_FUNCTION) {
            const auto * func_expr = static_cast<const FunctionExpression *>(select_item.expr.get());
            if (func_expr->is_aggregate()) {
                has_aggregate = true;
                break;
            }
        }
    }

    if (!bound_select_statement.group_by.empty() || has_aggregate) {
        // 复制 GROUP BY 表达式
        std::vector<std::unique_ptr<Expression>> group_by;
        group_by.reserve(bound_select_statement.group_by.size());
        for (const auto & group_expr : bound_select_statement.group_by) {
            group_by.push_back(copy_expression(group_expr.get()));
        }

        // 转换 SELECT 列表为聚合项
        std::vector<AggregateItem> aggregate_items;
        aggregate_items.reserve(bound_select_statement.select_items.size());
        for (const auto & select_item : bound_select_statement.select_items) {
            aggregate_items.push_back({
                copy_expression(select_item.expr.get()),
                select_item.alias.empty() ? std::optional<std::string>() : std::optional<std::string>(select_item.alias)
            });
        }

        auto aggregate = std::make_unique<LogicalAggregateNode>(
            std::move(group_by),
            std::move(aggregate_items)
        );
        aggregate->get_mutable_children().push_back(std::move(root));
        root = std::move(aggregate);
    }

    // 4. 如果有 HAVING 子句，添加 Filter 节点（在 Aggregate 之后）
    if (bound_select_statement.having) {
        auto having_filter = std::make_unique<LogicalFilterNode>(
            copy_expression(bound_select_statement.having.get())
        );
        having_filter->get_mutable_children().push_back(std::move(root));
        root = std::move(having_filter);
    }

    // 5. 添加 Project 节点（总是需要，用于投影 SELECT 列表）
    std::vector<ProjectItem> project_items;
    project_items.reserve(bound_select_statement.select_items.size());
    for (const auto & select_item : bound_select_statement.select_items) {
        project_items.push_back({
            copy_expression(select_item.expr.get()),
            select_item.alias.empty() ? std::optional<std::string>() : std::optional<std::string>(select_item.alias)
        });
    }

    // 如果没有聚合，才需要 Project 节点（如果有聚合，Project 已经在 Aggregate 中处理了）
    if (bound_select_statement.group_by.empty() && !has_aggregate) {
        auto project = std::make_unique<LogicalProjectNode>(std::move(project_items));
        project->get_mutable_children().push_back(std::move(root));
        root = std::move(project);
    }

    // 6. 如果有 ORDER BY，添加 Sort 节点
    if (!bound_select_statement.order_by.empty()) {
        std::vector<SortItem> sort_items;
        sort_items.reserve(bound_select_statement.order_by.size());
        for (const auto & order_item : bound_select_statement.order_by) {
            sort_items.push_back({
                copy_expression(order_item.expr.get()),
                order_item.order == Direction::ASC,
                false  // nulls_first，暂时设为 false，后续可以从 BoundOrderByItem 中获取
            });
        }

        auto sort = std::make_unique<LogicalSortNode>(std::move(sort_items));
        sort->get_mutable_children().push_back(std::move(root));
        root = std::move(sort);
    }

    // 7. 如果有 LIMIT 或 OFFSET，添加 LimitOffset 节点
    if (bound_select_statement.limit.has_value() || bound_select_statement.offset.has_value()) {
        auto limit_offset = std::make_unique<LogicalLimitOffsetNode>(
            bound_select_statement.limit,
            bound_select_statement.offset
        );
        limit_offset->get_mutable_children().push_back(std::move(root));
        root = std::move(limit_offset);
    }

    return root;
}

std::unique_ptr<LogicalPlanNode> LogicalPlanner::plan_delete(const BoundDeleteStatement & bound_delete_statement) const
{
    // 复制 WHERE 子句表达式
    auto where_expr = copy_expression(bound_delete_statement.where.get());

    return std::make_unique<LogicalDeletePlanNode>(
        bound_delete_statement.collection_id,
        std::move(where_expr)
    );
}

std::unique_ptr<LogicalPlanNode> LogicalPlanner::plan_update(const BoundUpdateStatement & bound_update_statement) const
{
    // 将 BoundUpdateItem 转换为 LogicalUpdatePlanNode::UpdateItem
    std::vector<LogicalUpdatePlanNode::UpdateItem> update_items;
    update_items.reserve(bound_update_statement.update_items.size());

    for (const auto & bound_item : bound_update_statement.update_items) {
        // 复制列引用表达式
        auto column_ref = copy_expression(bound_item.column_reference.get());

        // 复制值表达式
        auto value_expr = copy_expression(bound_item.value.get());

        update_items.push_back({
            std::move(column_ref),
            std::move(value_expr)
        });
    }

    // 复制 WHERE 子句表达式
    auto where_expr = copy_expression(bound_update_statement.where.get());

    return std::make_unique<LogicalUpdatePlanNode>(
        bound_update_statement.collection_id,
        std::move(update_items),
        std::move(where_expr)
    );
}

} // namespace dreamdb
