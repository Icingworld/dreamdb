#include "dreamdb/planner/logical_sort_node.h"

namespace dreamdb
{

LogicalSortNode::LogicalSortNode(std::vector<SortItem> sort_items)
    : LogicalPlanNode(LogicalPlanNodeType::SORT)
    , sort_items_(std::move(sort_items))
{
}

const std::vector<SortItem> & LogicalSortNode::get_sort_items() const noexcept
{
    return sort_items_;
}

std::vector<SortItem> & LogicalSortNode::get_mutable_sort_items() noexcept
{
    return sort_items_;
}

} // namespace dreamdb
