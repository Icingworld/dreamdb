#include "dreamdb/planner/logical/operator/logical_sort.h"

#include <cassert>

#include "dreamdb/binder/bound/expression/expression.h"

namespace dreamdb::planner::logical
{

LogicalSort::LogicalSort(std::vector<LogicalSortItem> sort_items)
    : LogicalOperator(LogicalOperatorType::Sort)
    , sort_items_(std::move(sort_items))
{
    // 排序项不能为空
    assert(!sort_items_.empty());

    // 注意：子节点是在创建后通过 add_child() 添加的
    // 因此不能在构造函数中检查 child_count()
}

std::size_t LogicalSort::sort_item_count() const noexcept
{
    return sort_items_.size();
}

const LogicalSortItem & LogicalSort::sort_item_at(std::size_t index) const noexcept
{
    // 排序项索引不能超过排序项数量
    assert(index < sort_items_.size());

    return sort_items_[index];
}

} // namespace dreamdb::planner::logical
