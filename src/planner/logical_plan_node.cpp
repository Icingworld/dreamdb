#include "dreamdb/planner/logical_plan_node.h"

namespace dreamdb
{

LogicalPlanNode::LogicalPlanNode(LogicalPlanNodeType type)
    : type_(type)
{
}

LogicalPlanNodeType LogicalPlanNode::get_type() const noexcept
{
    return type_;
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
