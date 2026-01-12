#include "dreamdb/planner/physical_planner/update/physical_update_plan_node.h"

namespace dreamdb
{

PhysicalUpdatePlanNode::PhysicalUpdatePlanNode(
    std::size_t collection_id,
    std::vector<UpdateItem> update_items,
    std::unique_ptr<PhysicalPlanNode> select_plan
)
    : PhysicalPlanNode(PhysicalPlanNodeOperationType::PHYSICAL_PLAN_UPDATE)
    , collection_id_(collection_id)
    , update_items_(std::move(update_items))
{
    // 将 SELECT 子计划作为第一个子节点
    if (select_plan) {
        children_.push_back(std::move(select_plan));
    }
}

std::size_t PhysicalUpdatePlanNode::get_collection_id() const noexcept
{
    return collection_id_;
}

const std::vector<PhysicalUpdatePlanNode::UpdateItem> & PhysicalUpdatePlanNode::get_update_items() const noexcept
{
    return update_items_;
}

const PhysicalPlanNode * PhysicalUpdatePlanNode::get_select_plan() const noexcept
{
    return children_.empty() ? nullptr : children_[0].get();
}

} // namespace dreamdb

