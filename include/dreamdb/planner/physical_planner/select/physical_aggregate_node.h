#pragma once

#include <string>
#include <vector>
#include <optional>
#include <memory>

#include "dreamdb/planner/physical_planner/select/physical_select_plan_node.h"
#include "dreamdb/expression/expression.h"

namespace dreamdb
{

/**
 * @brief 聚合项
 */
struct PhysicalAggregateItem
{
    std::unique_ptr<Expression> expression;    // 聚合表达式，如 COUNT(a) 等
    std::optional<std::string> alias;          // 可选别名
};

/**
 * @brief 聚合物理操作符
 */
class PhysicalAggregateNode : public PhysicalSelectPlanNode
{
public:
    explicit PhysicalAggregateNode(
        std::vector<std::unique_ptr<Expression>> group_by,
        std::vector<PhysicalAggregateItem> aggregate_items
    );

    PhysicalAggregateNode(const PhysicalAggregateNode &) = delete;
    PhysicalAggregateNode(PhysicalAggregateNode &&) noexcept = default;
    PhysicalAggregateNode & operator=(const PhysicalAggregateNode &) = delete;
    PhysicalAggregateNode & operator=(PhysicalAggregateNode &&) noexcept = default;

    ~PhysicalAggregateNode() override = default;

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
    const std::vector<PhysicalAggregateItem> & get_aggregate_items() const noexcept;

    /**
     * @brief 获取可变聚合项列表
     * @return 聚合项列表
     */
    std::vector<PhysicalAggregateItem> & get_mutable_aggregate_items() noexcept;

private:
    std::vector<std::unique_ptr<Expression>> group_by_;    // 分组表达式列表
    std::vector<PhysicalAggregateItem> aggregate_items_;   // 聚合项列表
};

} // namespace dreamdb

