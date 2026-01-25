#include "dreamdb/planner/logical/operator/logical_aggregate.h"

#include <cassert>

#include "dreamdb/binder/bound/expression/expression.h"

namespace dreamdb::planner::logical
{

LogicalAggregate::LogicalAggregate(std::vector<std::unique_ptr<
    dreamdb::binder::bound::BoundExpression>
> group_by, std::vector<LogicalAggregateItem> items)
    : LogicalOperator(LogicalOperatorType::Aggregate)
    , group_by_(std::move(group_by))
    , aggregate_items_(std::move(items))
{
    // 注意：当只有聚合函数而没有 GROUP BY 子句时，group_by_ 可以为空
    // 这表示对整个结果集进行聚合

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
