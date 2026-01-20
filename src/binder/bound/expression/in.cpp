#include "dreamdb/binder/bound/expression/in.h"

#include <cassert>

namespace dreamdb::binder::bound
{

BoundInExpression::BoundInExpression(
    std::unique_ptr<BoundExpression> left,
    std::vector<std::unique_ptr<BoundExpression>> values,
    dreamdb::common::LogicalType logical_type,
    bool is_not
)
    : BoundExpression(BoundExpressionType::In, logical_type)
    , left_(std::move(left))
    , values_(std::move(values))
    , is_not_(is_not)
{
    assert(left_ != nullptr);
}

const BoundExpression & BoundInExpression::left() const noexcept
{
    return *left_;
}

std::size_t BoundInExpression::value_count() const noexcept
{
    return values_.size();
}

const BoundExpression & BoundInExpression::value_at(std::size_t index) const noexcept
{
    assert(index < values_.size());
    assert(values_[index] != nullptr);
    return *values_[index];
}

bool BoundInExpression::is_not() const noexcept
{
    return is_not_;
}

} // namespace dreamdb::binder::bound
