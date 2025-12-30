#include "dreamdb/planner/logical_scan_node.h"

namespace dreamdb
{

LogicalScanNode::LogicalScanNode(std::size_t collection_id, std::vector<std::size_t> field_indexes)
    : LogicalPlanNode(LogicalPlanNodeType::SCAN)
    , collection_id_(collection_id)
    , field_indexes_(std::move(field_indexes))
{
}

std::size_t LogicalScanNode::get_collection_id() const noexcept
{
    return collection_id_;
}

const std::vector<std::size_t> & LogicalScanNode::get_field_indexes() const noexcept
{
    return field_indexes_;
}

} // namespace dreamdb
