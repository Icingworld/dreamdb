#include "dreamdb/planner/physical_planner/select/physical_seq_scan_node.h"

namespace dreamdb
{

PhysicalSeqScanNode::PhysicalSeqScanNode(
    std::size_t collection_id,
    std::vector<std::size_t> field_indexes
)
    : PhysicalSelectPlanNode(PhysicalSelectOperatorType::SELECT_SEQ_SCAN)
    , collection_id_(collection_id)
    , field_indexes_(std::move(field_indexes))
{
}

std::size_t PhysicalSeqScanNode::get_collection_id() const noexcept
{
    return collection_id_;
}

const std::vector<std::size_t> & PhysicalSeqScanNode::get_field_indexes() const noexcept
{
    return field_indexes_;
}

} // namespace dreamdb
