#include "dreamdb/planner/logical_planner/update/logical_update_plan_node.h"

namespace dreamdb
{

LogicalUpdatePlanNode::LogicalUpdatePlanNode(
    std::size_t collection_id,
    std::vector<UpdateItem> update_items,
    std::unique_ptr<Expression> where_clause
)
    : LogicalPlanNode(LogicalPlanNodeOperationType::LOGICAL_PLAN_UPDATE)
    , collection_id_(collection_id)
    , update_items_(std::move(update_items))
    , where_clause_(std::move(where_clause))
{
}

std::size_t LogicalUpdatePlanNode::get_collection_id() const noexcept
{
    return collection_id_;
}

const std::vector<LogicalUpdatePlanNode::UpdateItem> & LogicalUpdatePlanNode::get_update_items() const noexcept
{
    return update_items_;
}

const Expression * LogicalUpdatePlanNode::get_where_clause() const noexcept
{
    return where_clause_.get();
}

Expression * LogicalUpdatePlanNode::get_mutable_where_clause() noexcept
{
    return where_clause_.get();
}

} // namespace dreamdb

