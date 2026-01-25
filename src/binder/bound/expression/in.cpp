#include "dreamdb/binder/bound/expression/in.h"

#include <cassert>

#include "dreamdb/binder/bound/expression/visitor.h"

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

void BoundInExpression::accept(BoundExpressionVisitor & visitor) const
{
    visitor.visit(*this);
}

std::unique_ptr<BoundExpression> BoundInExpression::clone() const
{
    std::vector<std::unique_ptr<BoundExpression>> cloned_values;
    cloned_values.reserve(value_count());
    for (std::size_t i = 0; i < value_count(); ++i) {
        cloned_values.push_back(value_at(i).clone());
    }

    return std::make_unique<BoundInExpression>(
        left().clone(),
        std::move(cloned_values),
        logical_type(),
        is_not()
    );
}

} // namespace dreamdb::binder::bound
