#include "dreamdb/planner/physical_planner/select/physical_aggregate_node.h"

namespace dreamdb
{

PhysicalAggregateNode::PhysicalAggregateNode(
    std::vector<std::unique_ptr<Expression>> group_by,
    std::vector<PhysicalAggregateItem> aggregate_items
)
    : PhysicalSelectPlanNode(PhysicalSelectOperatorType::SELECT_AGGREGATE)
    , group_by_(std::move(group_by))
    , aggregate_items_(std::move(aggregate_items))
{
}

const std::vector<std::unique_ptr<Expression>> & PhysicalAggregateNode::get_group_by() const noexcept
{
    return group_by_;
}

std::vector<std::unique_ptr<Expression>> & PhysicalAggregateNode::get_mutable_group_by() noexcept
{
    return group_by_;
}

const std::vector<PhysicalAggregateItem> & PhysicalAggregateNode::get_aggregate_items() const noexcept
{
    return aggregate_items_;
}

std::vector<PhysicalAggregateItem> & PhysicalAggregateNode::get_mutable_aggregate_items() noexcept
{
    return aggregate_items_;
}

} // namespace dreamdb

