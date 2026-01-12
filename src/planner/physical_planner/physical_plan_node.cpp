#include "dreamdb/planner/physical_planner/physical_plan_node.h"

namespace dreamdb
{

PhysicalPlanNode::PhysicalPlanNode(PhysicalPlanNodeOperationType operation_type) noexcept
    : operation_type_(operation_type)
{
}

PhysicalPlanNodeOperationType PhysicalPlanNode::get_operation_type() const noexcept
{
    return operation_type_;
}

const std::vector<std::unique_ptr<PhysicalPlanNode>> & PhysicalPlanNode::get_children() const noexcept
{
    return children_;
}

std::vector<std::unique_ptr<PhysicalPlanNode>> & PhysicalPlanNode::get_mutable_children() noexcept
{
    return children_;
}

} // namespace dreamdb
