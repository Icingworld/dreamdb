#include "dreamdb/planner/logical/operator/logical_project.h"

#include <cassert>

namespace dreamdb::planner::logical
{

LogicalProject::LogicalProject(std::vector<LogicalProjectItem> project_items)
    : LogicalOperator(LogicalOperatorType::Project)
    , project_items_(std::move(project_items))
{
    // 投影项不能为空
    assert(!project_items_.empty());

    // Project 算子必须有且只有一个子算子
    assert(child_count() == 1);
}

std::size_t LogicalProject::project_item_count() const noexcept
{
    return project_items_.size();
}

const LogicalProjectItem & LogicalProject::project_item_at(std::size_t index) const noexcept
{
    // 投影项索引不能超过投影项数量
    assert(index < project_items_.size());

    return project_items_[index];
}

} // namespace dreamdb::planner::logical
