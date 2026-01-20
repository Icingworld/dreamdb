#include "dreamdb/binder/bound/expression/between.h"

#include <cassert>

namespace dreamdb::binder::bound
{

BoundBetweenExpression::BoundBetweenExpression(
    std::unique_ptr<BoundExpression> left,
    std::unique_ptr<BoundExpression> start,
    std::unique_ptr<BoundExpression> end,
    dreamdb::common::LogicalType logical_type,
    bool is_not
)
    : BoundExpression(BoundExpressionType::Between, logical_type)
    , left_(std::move(left))
    , start_(std::move(start))
    , end_(std::move(end))
    , is_not_(is_not)
{
    assert(left_ != nullptr);
    assert(start_ != nullptr);
    assert(end_ != nullptr);
}

const BoundExpression & BoundBetweenExpression::left() const noexcept
{
    return *left_;
}

const BoundExpression & BoundBetweenExpression::start() const noexcept
{
    return *start_;
}

const BoundExpression & BoundBetweenExpression::end() const noexcept
{
    return *end_;
}

bool BoundBetweenExpression::is_not() const noexcept
{
    return is_not_;
}

} // namespace dreamdb::binder::bound
