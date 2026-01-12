#include "dreamdb/planner/physical_planner/delete/physical_delete_plan_node.h"

namespace dreamdb
{

PhysicalDeletePlanNode::PhysicalDeletePlanNode(
    std::size_t collection_id,
    std::unique_ptr<PhysicalPlanNode> select_plan
)
    : PhysicalPlanNode(PhysicalPlanNodeOperationType::PHYSICAL_PLAN_DELETE)
    , collection_id_(collection_id)
{
    // 将 SELECT 子计划作为第一个子节点
    if (select_plan) {
        children_.push_back(std::move(select_plan));
    }
}

std::size_t PhysicalDeletePlanNode::get_collection_id() const noexcept
{
    return collection_id_;
}

const PhysicalPlanNode * PhysicalDeletePlanNode::get_select_plan() const noexcept
{
    return children_.empty() ? nullptr : children_[0].get();
}

} // namespace dreamdb

