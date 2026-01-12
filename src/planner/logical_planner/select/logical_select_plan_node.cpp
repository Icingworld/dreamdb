#include "dreamdb/planner/logical_planner/select/logical_select_plan_node.h"

namespace dreamdb
{

LogicalSelectPlanNode::LogicalSelectPlanNode(LogicalSelectPlanNodeType type) noexcept
    : LogicalPlanNode(LogicalPlanNodeOperationType::LOGICAL_PLAN_SELECT)
    , select_type_(type)
{
}

LogicalSelectPlanNodeType LogicalSelectPlanNode::get_select_type() const noexcept
{
    return select_type_;
}

} // namespace dreamdb
