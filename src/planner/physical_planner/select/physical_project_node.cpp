#include "dreamdb/planner/physical_planner/select/physical_project_node.h"

namespace dreamdb
{

PhysicalProjectNode::PhysicalProjectNode(std::vector<PhysicalProjectItem> project_items)
    : PhysicalSelectPlanNode(PhysicalSelectOperatorType::SELECT_PROJECT)
    , project_items_(std::move(project_items))
{
}

const std::vector<PhysicalProjectItem> & PhysicalProjectNode::get_project_items() const noexcept
{
    return project_items_;
}

std::vector<PhysicalProjectItem> & PhysicalProjectNode::get_mutable_project_items() noexcept
{
    return project_items_;
}

} // namespace dreamdb
