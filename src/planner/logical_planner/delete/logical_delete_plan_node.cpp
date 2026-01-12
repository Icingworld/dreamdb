#include "dreamdb/planner/logical_planner/delete/logical_delete_plan_node.h"

namespace dreamdb
{

LogicalDeletePlanNode::LogicalDeletePlanNode(
    std::size_t collection_id,
    std::unique_ptr<Expression> where_clause
)
    : LogicalPlanNode(LogicalPlanNodeOperationType::LOGICAL_PLAN_DELETE)
    , collection_id_(collection_id)
    , where_clause_(std::move(where_clause))
{
}

std::size_t LogicalDeletePlanNode::get_collection_id() const noexcept
{
    return collection_id_;
}

const Expression * LogicalDeletePlanNode::get_where_clause() const noexcept
{
    return where_clause_.get();
}

Expression * LogicalDeletePlanNode::get_mutable_where_clause() noexcept
{
    return where_clause_.get();
}

} // namespace dreamdb
