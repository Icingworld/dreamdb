#include "dreamdb/planner/logical_filter_node.h"

namespace dreamdb
{

LogicalFilterNode::LogicalFilterNode(std::unique_ptr<Expression> predicate)
    : LogicalPlanNode(LogicalPlanNodeType::FILTER)
    , predicate_(std::move(predicate))
{
}

const Expression & LogicalFilterNode::get_predicate() const noexcept
{
    return *predicate_;
}

Expression & LogicalFilterNode::get_mutable_predicate() noexcept
{
    return *predicate_;
}

} // namespace dreamdb
