#include "dreamdb/planner/physical_planner/physical_planner.h"

#include <stdexcept>
#include <unordered_set>
#include <algorithm>
#include <memory>
#include <vector>

#include "dreamdb/planner/logical_planner/select/logical_select_plan_node.h"
#include "dreamdb/planner/logical_planner/select/logical_scan_node.h"
#include "dreamdb/planner/logical_planner/select/logical_filter_node.h"
#include "dreamdb/planner/logical_planner/select/logical_project_node.h"
#include "dreamdb/planner/logical_planner/select/logical_aggregate_node.h"
#include "dreamdb/planner/logical_planner/select/logical_sort_node.h"
#include "dreamdb/planner/logical_planner/select/logical_limit_offset_node.h"
#include "dreamdb/planner/logical_planner/update/logical_update_plan_node.h"
#include "dreamdb/planner/logical_planner/delete/logical_delete_plan_node.h"
#include "dreamdb/planner/physical_planner/update/physical_update_plan_node.h"
#include "dreamdb/planner/physical_planner/delete/physical_delete_plan_node.h"
#include "dreamdb/planner/physical_planner/select/physical_seq_scan_node.h"
#include "dreamdb/planner/physical_planner/select/physical_filter_node.h"
#include "dreamdb/planner/physical_planner/select/physical_project_node.h"
#include "dreamdb/planner/physical_planner/select/physical_aggregate_node.h"
#include "dreamdb/planner/physical_planner/select/physical_sort_node.h"
#include "dreamdb/planner/physical_planner/select/physical_limit_offset_node.h"
#include "dreamdb/expression/expression.h"
#include "dreamdb/expression/column_reference_expression.h"
#include "dreamdb/expression/constant_expression.h"
#include "dreamdb/expression/binary_expression.h"
#include "dreamdb/expression/unary_expression.h"
#include "dreamdb/expression/function_expression.h"
#include "dreamdb/expression/in_expression.h"
#include "dreamdb/expression/like_expression.h"
#include "dreamdb/expression/between_expression.h"
#include "dreamdb/expression/null_expression.h"

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

PhysicalPlanner::PhysicalPlanner(const Catalog & catalog) noexcept
    : catalog_(catalog)
{
}

std::unique_ptr<PhysicalPlanNode> PhysicalPlanner::plan(const LogicalPlanNode & logical_plan) const
{
    // 根据逻辑计划的操作类型，选择对应的物理计划
    switch (logical_plan.get_operation_type()) {
        case LogicalPlanNodeOperationType::LOGICAL_PLAN_SELECT:
            return plan_select(static_cast<const LogicalSelectPlanNode &>(logical_plan));
        case LogicalPlanNodeOperationType::LOGICAL_PLAN_UPDATE:
            return plan_update(static_cast<const LogicalUpdatePlanNode &>(logical_plan));
        case LogicalPlanNodeOperationType::LOGICAL_PLAN_DELETE:
            return plan_delete(static_cast<const LogicalDeletePlanNode &>(logical_plan));
        default:
            throw std::runtime_error("Unsupported logical plan operation type: " + 
                std::to_string(static_cast<int>(logical_plan.get_operation_type())));
    }
}

std::unique_ptr<PhysicalPlanNode> PhysicalPlanner::plan_select(const LogicalSelectPlanNode & logical_node) const
{
    // 根据逻辑节点类型进行转换
    switch (logical_node.get_select_type()) {
        case LogicalSelectPlanNodeType::SELECT_SCAN: {
            const auto & scan_node = static_cast<const LogicalScanNode &>(logical_node);
            // 目前只支持全表扫描，后续可以优化为索引扫描
            return std::make_unique<PhysicalSeqScanNode>(
                scan_node.get_collection_id(),
                scan_node.get_field_indexes()
            );
        }

        case LogicalSelectPlanNodeType::SELECT_FILTER: {
            const auto & filter_node = static_cast<const LogicalFilterNode &>(logical_node);
            
            // 递归转换子节点
            if (filter_node.get_children().empty()) {
                throw std::runtime_error("Filter node must have a child node");
            }
            auto child_plan = plan_select(static_cast<const LogicalSelectPlanNode &>(*filter_node.get_children()[0]));
            
            // 复制谓词表达式
            auto predicate = copy_expression(&filter_node.get_predicate());
            
            // 创建物理 Filter 节点
            auto physical_filter = std::make_unique<PhysicalFilterNode>(std::move(predicate));
            physical_filter->get_mutable_children().push_back(std::move(child_plan));
            
            return physical_filter;
        }

        case LogicalSelectPlanNodeType::SELECT_PROJECT: {
            const auto & project_node = static_cast<const LogicalProjectNode &>(logical_node);
            
            // 递归转换子节点
            if (project_node.get_children().empty()) {
                throw std::runtime_error("Project node must have a child node");
            }
            auto child_plan = plan_select(static_cast<const LogicalSelectPlanNode &>(*project_node.get_children()[0]));
            
            // 转换投影项
            std::vector<PhysicalProjectItem> project_items;
            project_items.reserve(project_node.get_project_items().size());
            for (const auto & logical_item : project_node.get_project_items()) {
                project_items.push_back({
                    copy_expression(logical_item.expression.get()),
                    logical_item.alias
                });
            }
            
            // 创建物理 Project 节点
            auto physical_project = std::make_unique<PhysicalProjectNode>(std::move(project_items));
            physical_project->get_mutable_children().push_back(std::move(child_plan));
            
            return physical_project;
        }

        case LogicalSelectPlanNodeType::SELECT_AGGREGATE: {
            const auto & aggregate_node = static_cast<const LogicalAggregateNode &>(logical_node);
            
            // 递归转换子节点
            if (aggregate_node.get_children().empty()) {
                throw std::runtime_error("Aggregate node must have a child node");
            }
            auto child_plan = plan_select(static_cast<const LogicalSelectPlanNode &>(*aggregate_node.get_children()[0]));
            
            // 复制 GROUP BY 表达式
            std::vector<std::unique_ptr<Expression>> group_by;
            group_by.reserve(aggregate_node.get_group_by().size());
            for (const auto & group_expr : aggregate_node.get_group_by()) {
                group_by.push_back(copy_expression(group_expr.get()));
            }
            
            // 转换聚合项
            std::vector<PhysicalAggregateItem> aggregate_items;
            aggregate_items.reserve(aggregate_node.get_aggregate_items().size());
            for (const auto & logical_item : aggregate_node.get_aggregate_items()) {
                aggregate_items.push_back({
                    copy_expression(logical_item.expression.get()),
                    logical_item.alias
                });
            }
            
            // 创建物理 Aggregate 节点
            auto physical_aggregate = std::make_unique<PhysicalAggregateNode>(
                std::move(group_by),
                std::move(aggregate_items)
            );
            physical_aggregate->get_mutable_children().push_back(std::move(child_plan));
            
            return physical_aggregate;
        }

        case LogicalSelectPlanNodeType::SELECT_SORT: {
            const auto & sort_node = static_cast<const LogicalSortNode &>(logical_node);
            
            // 递归转换子节点
            if (sort_node.get_children().empty()) {
                throw std::runtime_error("Sort node must have a child node");
            }
            auto child_plan = plan_select(static_cast<const LogicalSelectPlanNode &>(*sort_node.get_children()[0]));
            
            // 转换排序项
            std::vector<PhysicalSortItem> sort_items;
            sort_items.reserve(sort_node.get_sort_items().size());
            for (const auto & logical_item : sort_node.get_sort_items()) {
                sort_items.push_back({
                    copy_expression(logical_item.expression.get()),
                    logical_item.ascending,
                    logical_item.nulls_first
                });
            }
            
            // 创建物理 Sort 节点
            auto physical_sort = std::make_unique<PhysicalSortNode>(std::move(sort_items));
            physical_sort->get_mutable_children().push_back(std::move(child_plan));
            
            return physical_sort;
        }

        case LogicalSelectPlanNodeType::SELECT_LIMIT_OFFSET: {
            const auto & limit_offset_node = static_cast<const LogicalLimitOffsetNode &>(logical_node);
            
            // 递归转换子节点
            if (limit_offset_node.get_children().empty()) {
                throw std::runtime_error("LimitOffset node must have a child node");
            }
            auto child_plan = plan_select(static_cast<const LogicalSelectPlanNode &>(*limit_offset_node.get_children()[0]));
            
            // 创建物理 LimitOffset 节点
            auto physical_limit_offset = std::make_unique<PhysicalLimitOffsetNode>(
                limit_offset_node.get_limit(),
                limit_offset_node.get_offset()
            );
            physical_limit_offset->get_mutable_children().push_back(std::move(child_plan));
            
            return physical_limit_offset;
        }

        default:
            throw std::runtime_error("Unsupported logical select plan node type: " + 
                std::to_string(static_cast<int>(logical_node.get_select_type())));
    }
}

std::unique_ptr<PhysicalPlanNode> PhysicalPlanner::plan_update(const LogicalUpdatePlanNode & logical_node) const
{
    // 收集所有需要的字段索引
    // 注意：UPDATE 的 SELECT 子计划只需要 WHERE 子句来定位要更新的行
    // 但我们也需要收集 UPDATE 项中的列引用，因为执行器需要知道要更新哪些列
    std::unordered_set<std::size_t> field_index_set;

    // 从 WHERE 子句中收集（用于定位要更新的行）
    if (logical_node.get_where_clause()) {
        collect_column_references(logical_node.get_where_clause(), field_index_set);
    }

    // 从 UPDATE 项中收集（列引用和值表达式中的列引用）
    // 这些列需要被扫描，以便执行器能够更新它们
    for (const auto & update_item : logical_node.get_update_items()) {
        collect_column_references(update_item.column_reference.get(), field_index_set);
        collect_column_references(update_item.value.get(), field_index_set);
    }

    // 转换为有序的字段索引列表
    std::vector<std::size_t> field_indexes(field_index_set.begin(), field_index_set.end());
    std::sort(field_indexes.begin(), field_indexes.end());

    // 构建 SELECT 子计划：只包含 Scan + Filter（WHERE 子句）
    // 不需要 Project、Aggregate、Sort、Limit 等节点
    std::unique_ptr<PhysicalPlanNode> select_plan = std::make_unique<PhysicalSeqScanNode>(
        logical_node.get_collection_id(),
        field_indexes
    );

    // 如果有 WHERE 子句，添加 Filter 节点
    if (logical_node.get_where_clause()) {
        auto filter_predicate = copy_expression(logical_node.get_where_clause());
        auto filter = std::make_unique<PhysicalFilterNode>(std::move(filter_predicate));
        filter->get_mutable_children().push_back(std::move(select_plan));
        select_plan = std::move(filter);
    }

    // 复制 UPDATE 项
    std::vector<PhysicalUpdatePlanNode::UpdateItem> update_items;
    update_items.reserve(logical_node.get_update_items().size());
    for (const auto & logical_item : logical_node.get_update_items()) {
        update_items.push_back({
            copy_expression(logical_item.column_reference.get()),
            copy_expression(logical_item.value.get())
        });
    }

    // 创建 PhysicalUpdatePlanNode
    auto update_plan = std::make_unique<PhysicalUpdatePlanNode>(
        logical_node.get_collection_id(),
        std::move(update_items),
        std::move(select_plan)
    );

    return update_plan;
}

std::unique_ptr<PhysicalPlanNode> PhysicalPlanner::plan_delete(const LogicalDeletePlanNode & logical_node) const
{
    // 收集所有需要的字段索引（从 WHERE 子句中）
    // 注意：DELETE 的 SELECT 子计划只需要 WHERE 子句来定位要删除的行
    // 不需要 SELECT 列表、GROUP BY、ORDER BY、LIMIT 等
    std::unordered_set<std::size_t> field_index_set;

    if (logical_node.get_where_clause()) {
        collect_column_references(logical_node.get_where_clause(), field_index_set);
    }

    // 转换为有序的字段索引列表
    std::vector<std::size_t> field_indexes(field_index_set.begin(), field_index_set.end());
    std::sort(field_indexes.begin(), field_indexes.end());

    // 构建 SELECT 子计划：只包含 Scan + Filter（WHERE 子句）
    // 不需要 Project、Aggregate、Sort、Limit 等节点
    std::unique_ptr<PhysicalPlanNode> select_plan = std::make_unique<PhysicalSeqScanNode>(
        logical_node.get_collection_id(),
        field_indexes
    );

    // 如果有 WHERE 子句，添加 Filter 节点
    if (logical_node.get_where_clause()) {
        auto filter_predicate = copy_expression(logical_node.get_where_clause());
        auto filter = std::make_unique<PhysicalFilterNode>(std::move(filter_predicate));
        filter->get_mutable_children().push_back(std::move(select_plan));
        select_plan = std::move(filter);
    }

    // 创建 PhysicalDeletePlanNode
    auto delete_plan = std::make_unique<PhysicalDeletePlanNode>(
        logical_node.get_collection_id(),
        std::move(select_plan)
    );

    return delete_plan;
}

} // namespace dreamdb
