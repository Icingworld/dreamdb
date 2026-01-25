#include "dreamdb/planner/logical/operator/logical_project.h"

#include <cassert>

#include "dreamdb/binder/bound/expression/expression.h"

namespace dreamdb::planner::logical
{

LogicalProject::LogicalProject(std::vector<LogicalProjectItem> project_items)
    : LogicalOperator(LogicalOperatorType::Project)
    , project_items_(std::move(project_items))
{
    // 投影项不能为空
    assert(!project_items_.empty());

    // 注意：子节点是在创建后通过 add_child() 添加的
    // 因此不能在构造函数中检查 child_count()
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
