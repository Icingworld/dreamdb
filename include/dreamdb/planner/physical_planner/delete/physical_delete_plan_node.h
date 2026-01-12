#pragma once

#include <cstddef>
#include <memory>

#include "dreamdb/planner/physical_planner/physical_plan_node.h"

namespace dreamdb
{

/**
 * @brief DELETE 物理计划节点
 * @details DELETE 操作包含一个 SELECT 子计划（用于找到要删除的行）
 * 执行流程：先通过 SELECT 子计划找到满足条件的行，然后对这些行执行删除操作
 */
class PhysicalDeletePlanNode : public PhysicalPlanNode
{
public:
    explicit PhysicalDeletePlanNode(
        std::size_t collection_id,
        std::unique_ptr<PhysicalPlanNode> select_plan  // SELECT 子计划，用于找到要删除的行
    );

    PhysicalDeletePlanNode(const PhysicalDeletePlanNode &) = delete;
    PhysicalDeletePlanNode(PhysicalDeletePlanNode &&) noexcept = default;
    PhysicalDeletePlanNode & operator=(const PhysicalDeletePlanNode &) = delete;
    PhysicalDeletePlanNode & operator=(PhysicalDeletePlanNode &&) noexcept = default;

    ~PhysicalDeletePlanNode() override = default;

public:
    /**
     * @brief 获取集合 ID
     * @return 集合 ID
     */
    std::size_t get_collection_id() const noexcept;

    /**
     * @brief 获取 SELECT 子计划
     * @return SELECT 子计划，用于找到要删除的行
     */
    const PhysicalPlanNode * get_select_plan() const noexcept;

private:
    std::size_t collection_id_;                // 集合 ID
    // 注意：SELECT 子计划存储在 children_ 中（作为第一个子节点）
};

} // namespace dreamdb
