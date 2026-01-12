#include "dreamdb/planner/logical_planner/select/logical_offset_node.h"

namespace dreamdb
{

LogicalOffsetNode::LogicalOffsetNode(std::size_t offset) noexcept
    : LogicalSelectPlanNode(LogicalSelectPlanNodeType::SELECT_OFFSET)
    , offset_(offset)
{
}

std::size_t LogicalOffsetNode::get_offset() const noexcept
{
    return offset_;
}

} // namespace dreamdb
