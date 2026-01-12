#include "dreamdb/planner/physical_planner/select/physical_filter_node.h"

namespace dreamdb
{

PhysicalFilterNode::PhysicalFilterNode(std::unique_ptr<Expression> predicate)
    : PhysicalSelectPlanNode(PhysicalSelectOperatorType::SELECT_FILTER)
    , predicate_(std::move(predicate))
{
}

const Expression & PhysicalFilterNode::get_predicate() const noexcept
{
    return *predicate_;
}

Expression & PhysicalFilterNode::get_mutable_predicate() noexcept
{
    return *predicate_;
}

} // namespace dreamdb
