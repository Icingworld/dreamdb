#pragma once

#include <cstddef>
#include <vector>
#include <memory>

#include "dreamdb/planner/physical_planner/physical_plan_node.h"
#include "dreamdb/expression/expression.h"

namespace dreamdb
{

/**
 * @brief UPDATE 物理计划节点
 * @details UPDATE 操作包含一个 SELECT 子计划（用于找到要更新的行）和更新项列表
 * 执行流程：先通过 SELECT 子计划找到满足条件的行，然后对这些行执行更新操作
 */
class PhysicalUpdatePlanNode : public PhysicalPlanNode
{
public:
    /**
     * @brief 更新项
     */
    struct UpdateItem
    {
        std::unique_ptr<Expression> column_reference;   // 列引用表达式
        std::unique_ptr<Expression> value;             // 值表达式
    };

public:
    explicit PhysicalUpdatePlanNode(
        std::size_t collection_id,
        std::vector<UpdateItem> update_items,
        std::unique_ptr<PhysicalPlanNode> select_plan  // SELECT 子计划，用于找到要更新的行
    );

    PhysicalUpdatePlanNode(const PhysicalUpdatePlanNode &) = delete;
    PhysicalUpdatePlanNode(PhysicalUpdatePlanNode &&) noexcept = default;
    PhysicalUpdatePlanNode & operator=(const PhysicalUpdatePlanNode &) = delete;
    PhysicalUpdatePlanNode & operator=(PhysicalUpdatePlanNode &&) noexcept = default;

    ~PhysicalUpdatePlanNode() override = default;

public:
    /**
     * @brief 获取集合 ID
     * @return 集合 ID
     */
    std::size_t get_collection_id() const noexcept;

    /**
     * @brief 获取更新项列表
     * @return 更新项列表
     */
    const std::vector<UpdateItem> & get_update_items() const noexcept;

    /**
     * @brief 获取 SELECT 子计划
     * @return SELECT 子计划，用于找到要更新的行
     */
    const PhysicalPlanNode * get_select_plan() const noexcept;

private:
    std::size_t collection_id_;                // 集合 ID
    std::vector<UpdateItem> update_items_;     // 更新项列表
    // 注意：SELECT 子计划存储在 children_ 中（作为第一个子节点）
};

} // namespace dreamdb
