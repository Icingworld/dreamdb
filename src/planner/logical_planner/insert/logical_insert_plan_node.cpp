#include "dreamdb/planner/logical_planner/insert/logical_insert_plan_node.h"

namespace dreamdb
{

LogicalInsertPlanNode::LogicalInsertPlanNode(
    std::size_t collection_id,
    std::vector<InsertItem> insert_items
)
    : LogicalPlanNode(LogicalPlanNodeOperationType::LOGICAL_PLAN_INSERT)
    , collection_id_(collection_id)
    , insert_items_(std::move(insert_items))
{
}

std::size_t LogicalInsertPlanNode::get_collection_id() const noexcept
{
    return collection_id_;
}

const std::vector<LogicalInsertPlanNode::InsertItem> & LogicalInsertPlanNode::get_insert_items() const noexcept
{
    return insert_items_;
}

} // namespace dreamdb
