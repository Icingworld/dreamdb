#include "dreamdb/planner/physical_planner/select/physical_limit_offset_node.h"

namespace dreamdb
{

PhysicalLimitOffsetNode::PhysicalLimitOffsetNode(
    std::optional<std::size_t> limit,
    std::optional<std::size_t> offset
) noexcept
    : PhysicalSelectPlanNode(PhysicalSelectOperatorType::SELECT_LIMIT_OFFSET)
    , limit_(limit)
    , offset_(offset)
{
}

std::optional<std::size_t> PhysicalLimitOffsetNode::get_limit() const noexcept
{
    return limit_;
}

std::optional<std::size_t> PhysicalLimitOffsetNode::get_offset() const noexcept
{
    return offset_;
}

} // namespace dreamdb
