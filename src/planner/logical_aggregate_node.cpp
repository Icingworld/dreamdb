#include "dreamdb/planner/logical_aggregate_node.h"

namespace dreamdb
{

LogicalAggregateNode::LogicalAggregateNode(std::vector<std::unique_ptr<Expression>> group_by, std::vector<AggregateItem> aggregate_items)
    : LogicalPlanNode(LogicalPlanNodeType::AGGREGATE)
    , group_by_(std::move(group_by))
    , aggregate_items_(std::move(aggregate_items))
{
}

const std::vector<std::unique_ptr<Expression>> & LogicalAggregateNode::get_group_by() const noexcept
{
    return group_by_;
}

std::vector<std::unique_ptr<Expression>> & LogicalAggregateNode::get_mutable_group_by() noexcept
{
    return group_by_;
}

const std::vector<AggregateItem> & LogicalAggregateNode::get_aggregate_items() const noexcept
{
    return aggregate_items_;
}

std::vector<AggregateItem> & LogicalAggregateNode::get_mutable_aggregate_items() noexcept
{
    return aggregate_items_;
}

} // namespace dreamdb
