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

} // namespace dreamdb
