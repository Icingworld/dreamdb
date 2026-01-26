#include "dreamdb/planner/physical/operator/physical_limit_offset.h"

namespace dreamdb::planner::physical
{

PhysicalLimitOffset::PhysicalLimitOffset(
    std::unique_ptr<PhysicalOperator> child,
    std::optional<std::size_t> limit,
    std::optional<std::size_t> offset
)
    : PhysicalUnaryOperator(std::move(child))
    , limit_(limit)
    , offset_(offset)
    , skipped_count_(0)
    , returned_count_(0)
{
}

void PhysicalLimitOffset::open(ExecutionContext & context)
{
    PhysicalUnaryOperator::open(context);
    skipped_count_ = 0;
    returned_count_ = 0;
}

bool PhysicalLimitOffset::next(ExecutionContext & context, Row & row)
{
    // 首先跳过 offset 行
    while (offset_.has_value() && skipped_count_ < offset_.value()) {
        if (!child_->next(context, row)) {
            // 如果子算子没有更多行，直接返回 false
            return false;
        }
        ++skipped_count_;
    }

    // 检查是否已经达到 limit
    if (limit_.has_value() && returned_count_ >= limit_.value()) {
        return false;
    }

    // 从子算子获取下一行
    if (!child_->next(context, row)) {
        return false;
    }

    // 增加返回计数
    ++returned_count_;
    return true;
}

void PhysicalLimitOffset::close(ExecutionContext & context)
{
    PhysicalUnaryOperator::close(context);
    skipped_count_ = 0;
    returned_count_ = 0;
}

} // namespace dreamdb::planner::physical
