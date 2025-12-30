#include "dreamdb/planner/logical_limit_node.h"

namespace dreamdb
{

LogicalLimitNode::LogicalLimitNode(std::size_t limit) noexcept
    : LogicalPlanNode(LogicalPlanNodeType::LIMIT)
    , limit_(limit)
{
}

std::size_t LogicalLimitNode::get_limit() const noexcept
{
    return limit_;
}

} // namespace dreamdb
