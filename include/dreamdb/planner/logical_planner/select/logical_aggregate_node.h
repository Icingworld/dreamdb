#pragma once

#include <string>
#include <vector>
#include <optional>
#include <memory>

#include "dreamdb/planner/logical_planner/select/logical_select_plan_node.h"
#include "dreamdb/expression/expression.h"

namespace dreamdb
{

/**
 * @brief 聚合项
 */
struct AggregateItem
{
    std::unique_ptr<Expression> expression;    // 聚合表达式，如 COUNT(a) 等
    std::optional<std::string> alias;          // 可选别名
};

/**
 * @brief 聚合节点
 */
class LogicalAggregateNode : public LogicalSelectPlanNode
{
public:
    explicit LogicalAggregateNode(std::vector<std::unique_ptr<Expression>> group_by, std::vector<AggregateItem> aggregate_items);

    LogicalAggregateNode(const LogicalAggregateNode &) noexcept = delete;

    LogicalAggregateNode(LogicalAggregateNode &&) noexcept = default;

    LogicalAggregateNode & operator=(const LogicalAggregateNode &) noexcept = delete;

    LogicalAggregateNode & operator=(LogicalAggregateNode &&) noexcept = default;

    ~LogicalAggregateNode() noexcept = default;

public:
    /**
     * @brief 获取分组表达式列表
     * @return 分组表达式列表
     */
    const std::vector<std::unique_ptr<Expression>> & get_group_by() const noexcept;

    /**
     * @brief 获取可变分组表达式列表
     * @return 分组表达式列表
     */
    std::vector<std::unique_ptr<Expression>> & get_mutable_group_by() noexcept;

    /**
     * @brief 获取聚合项列表
     * @return 聚合项列表
     */
    const std::vector<AggregateItem> & get_aggregate_items() const noexcept;

    /**
     * @brief 获取可变聚合项列表
     * @return 聚合项列表
     */
    std::vector<AggregateItem> & get_mutable_aggregate_items() noexcept;

private:
    std::vector<std::unique_ptr<Expression>> group_by_;    // 分组表达式列表
    std::vector<AggregateItem> aggregate_items_;           // 聚合项列表
};

} // namespace dreamdb
