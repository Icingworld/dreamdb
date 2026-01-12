#include "dreamdb/planner/logical_planner/logical_plan_node.h"

namespace dreamdb
{

LogicalPlanNode::LogicalPlanNode(LogicalPlanNodeOperationType operation_type) noexcept
    : operation_type_(operation_type)
{
}

LogicalPlanNodeOperationType LogicalPlanNode::get_operation_type() const noexcept
{
    return operation_type_;
}

const std::vector<std::unique_ptr<LogicalPlanNode>> & LogicalPlanNode::get_children() const noexcept
{
    return children_;
}

std::vector<std::unique_ptr<LogicalPlanNode>> & LogicalPlanNode::get_mutable_children() noexcept
{
    return children_;
}

} // namespace dreamdb
