#include "dreamdb/planner/logical_project_node.h"

namespace dreamdb
{

LogicalProjectNode::LogicalProjectNode(std::vector<ProjectItem> project_items)
    : LogicalPlanNode(LogicalPlanNodeType::PROJECT)
    , project_items_(std::move(project_items))
{
}

const std::vector<ProjectItem> & LogicalProjectNode::get_project_items() const noexcept
{
    return project_items_;
}

std::vector<ProjectItem> & LogicalProjectNode::get_mutable_project_items() noexcept
{
    return project_items_;
}

} // namespace dreamdb
