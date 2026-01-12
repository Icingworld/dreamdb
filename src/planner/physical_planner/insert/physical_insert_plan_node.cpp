#include "dreamdb/planner/physical_planner/insert/physical_insert_plan_node.h"

namespace dreamdb
{

PhysicalInsertPlanNode::PhysicalInsertPlanNode(
    std::size_t collection_id,
    std::vector<InsertItem> insert_items
)
    : PhysicalPlanNode(PhysicalPlanNodeOperationType::PHYSICAL_PLAN_INSERT)
    , collection_id_(collection_id)
    , insert_items_(std::move(insert_items))
{
}

std::size_t PhysicalInsertPlanNode::get_collection_id() const noexcept
{
    return collection_id_;
}

const std::vector<PhysicalInsertPlanNode::InsertItem> & PhysicalInsertPlanNode::get_insert_items() const noexcept
{
    return insert_items_;
}

} // namespace dreamdb
