#include "dreamdb/binder/bound/expression/like.h"

#include <cassert>

namespace dreamdb::binder::bound
{

BoundLikeExpression::BoundLikeExpression(
    std::unique_ptr<BoundExpression> left,
    std::unique_ptr<BoundExpression> pattern,
    dreamdb::common::LogicalType logical_type,
    bool is_not
)
    : BoundExpression(BoundExpressionType::Like, logical_type)
    , left_(std::move(left))
    , pattern_(std::move(pattern))
    , is_not_(is_not)
{
    assert(left_ != nullptr);
    assert(pattern_ != nullptr);
}

const BoundExpression & BoundLikeExpression::left() const noexcept
{
    return *left_;
}

const BoundExpression & BoundLikeExpression::pattern() const noexcept
{
    return *pattern_;
}

bool BoundLikeExpression::is_not() const noexcept
{
    return is_not_;
}

} // namespace dreamdb::binder::bound
