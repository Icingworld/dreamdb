#include "dreamdb/planner/physical_planner/select/physical_select_plan_node.h"

namespace dreamdb
{

PhysicalSelectPlanNode::PhysicalSelectPlanNode(PhysicalSelectOperatorType operator_type) noexcept
    : PhysicalPlanNode(PhysicalPlanNodeOperationType::PHYSICAL_PLAN_SELECT)
    , operator_type_(operator_type)
{
}

PhysicalSelectOperatorType PhysicalSelectPlanNode::get_operator_type() const noexcept
{
    return operator_type_;
}

} // namespace dreamdb
