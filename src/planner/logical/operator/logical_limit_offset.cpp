#include "dreamdb/planner/logical/operator/logical_limit_offset.h"

#include <cassert>

namespace dreamdb::planner::logical
{

LogicalLimitOffset::LogicalLimitOffset(std::optional<std::size_t> limit, std::optional<std::size_t> offset)
    : LogicalOperator(LogicalOperatorType::LimitOffset)
    , limit_(limit)
    , offset_(offset)
{
    // 限制和偏移不能同时为空
    assert(limit.has_value() || offset.has_value());

    // 注意：子节点是在创建后通过 add_child() 添加的
    // 因此不能在构造函数中检查 child_count()
}

std::optional<std::size_t> LogicalLimitOffset::limit() const noexcept
{
    return limit_;
}

std::optional<std::size_t> LogicalLimitOffset::offset() const noexcept
{
    return offset_;
}

} // namespace dreamdb::planner::logical
