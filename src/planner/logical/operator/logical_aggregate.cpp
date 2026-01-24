#include "dreamdb/planner/logical/operator/logical_aggregate.h"

#include <cassert>

namespace dreamdb::planner::logical
{

LogicalAggregate::LogicalAggregate(std::vector<std::unique_ptr<
    dreamdb::binder::bound::BoundExpression>
> group_by, std::vector<LogicalAggregateItem> items)
    : LogicalOperator(LogicalOperatorType::Aggregate)
    , group_by_(std::move(group_by))
    , aggregate_items_(std::move(items))
{
    // 分组表达式不能为空
    assert(!group_by_.empty());

    // 聚合表达式项不能为空
    assert(!aggregate_items_.empty());
}

std::size_t LogicalAggregate::group_by_count() const noexcept
{
    return group_by_.size();
}

const dreamdb::binder::bound::BoundExpression & LogicalAggregate::group_by_at(std::size_t index) const noexcept
{
    // 分组表达式索引不能超过分组表达式数量
    assert(index < group_by_.size());

    return *group_by_[index];
}

std::size_t LogicalAggregate::aggregate_items_count() const noexcept
{
    return aggregate_items_.size();
}

const LogicalAggregateItem & LogicalAggregate::aggregate_item_at(std::size_t index) const noexcept
{
    return aggregate_items_[index];
}

} // namespace dreamdb::planner::logical
