#include "dreamdb/planner/logical_planner/select/logical_limit_offset_node.h"

namespace dreamdb
{

LogicalLimitOffsetNode::LogicalLimitOffsetNode(
    std::optional<std::size_t> limit,
    std::optional<std::size_t> offset
) noexcept
    : LogicalSelectPlanNode(LogicalSelectPlanNodeType::SELECT_LIMIT_OFFSET)
    , limit_(limit)
    , offset_(offset)
{
}

std::optional<std::size_t> LogicalLimitOffsetNode::get_limit() const noexcept
{
    return limit_;
}

std::optional<std::size_t> LogicalLimitOffsetNode::get_offset() const noexcept
{
    return offset_;
}

} // namespace dreamdb

