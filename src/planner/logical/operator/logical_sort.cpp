#include "dreamdb/planner/logical/operator/logical_sort.h"

#include <cassert>

namespace dreamdb::planner::logical
{

LogicalSort::LogicalSort(std::vector<LogicalSortItem> sort_items)
    : LogicalOperator(LogicalOperatorType::Sort)
    , sort_items_(std::move(sort_items))
{
    // 排序项不能为空
    assert(!sort_items_.empty());

    // Sort 算子必须有且只有一个子算子
    assert(child_count() == 1);
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
