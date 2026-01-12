#include "dreamdb/planner/physical_planner/select/physical_index_scan_node.h"

namespace dreamdb
{

PhysicalIndexScanNode::PhysicalIndexScanNode(
    std::size_t collection_id,
    const std::string & index_name,
    std::vector<std::size_t> field_indexes
)
    : PhysicalSelectPlanNode(PhysicalSelectOperatorType::SELECT_INDEX_SCAN)
    , collection_id_(collection_id)
    , index_name_(index_name)
    , field_indexes_(std::move(field_indexes))
{
}

std::size_t PhysicalIndexScanNode::get_collection_id() const noexcept
{
    return collection_id_;
}

const std::string & PhysicalIndexScanNode::get_index_name() const noexcept
{
    return index_name_;
}

const std::vector<std::size_t> & PhysicalIndexScanNode::get_field_indexes() const noexcept
{
    return field_indexes_;
}

} // namespace dreamdb
