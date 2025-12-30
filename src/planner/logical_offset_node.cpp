#include "dreamdb/planner/logical_offset_node.h"

namespace dreamdb
{

LogicalOffsetNode::LogicalOffsetNode(std::size_t offset) noexcept
    : LogicalPlanNode(LogicalPlanNodeType::OFFSET)
    , offset_(offset)
{
}

std::size_t LogicalOffsetNode::get_offset() const noexcept
{
    return offset_;
}

} // namespace dreamdb
