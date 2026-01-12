#include "dreamdb/planner/physical_planner/select/physical_sort_node.h"

namespace dreamdb
{

PhysicalSortNode::PhysicalSortNode(std::vector<PhysicalSortItem> sort_items)
    : PhysicalSelectPlanNode(PhysicalSelectOperatorType::SELECT_SORT)
    , sort_items_(std::move(sort_items))
{
}

const std::vector<PhysicalSortItem> & PhysicalSortNode::get_sort_items() const noexcept
{
    return sort_items_;
}

std::vector<PhysicalSortItem> & PhysicalSortNode::get_mutable_sort_items() noexcept
{
    return sort_items_;
}

} // namespace dreamdb
