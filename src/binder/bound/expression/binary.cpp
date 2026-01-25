#include "dreamdb/binder/bound/expression/binary.h"

#include <cassert>

#include "dreamdb/binder/bound/expression/visitor.h"

namespace dreamdb::binder::bound
{

BoundBinaryExpression::BoundBinaryExpression(
    BoundBinaryOperatorType operator_type,
    std::unique_ptr<BoundExpression> left,
    std::unique_ptr<BoundExpression> right,
    dreamdb::common::LogicalType logical_type
)
    : BoundExpression(BoundExpressionType::Binary, logical_type)
    , operator_type_(operator_type)
    , left_(std::move(left))
    , right_(std::move(right))
{
    // 左操作数和右操作数不能为空
    assert(left_ != nullptr);
    assert(right_ != nullptr);
}

BoundBinaryOperatorType BoundBinaryExpression::operator_type() const noexcept
{
    return operator_type_;
}

const BoundExpression & BoundBinaryExpression::left() const noexcept
{
    return *left_;
}

const BoundExpression & BoundBinaryExpression::right() const noexcept
{
    return *right_;
}

void BoundBinaryExpression::accept(BoundExpressionVisitor & visitor) const
{
    visitor.visit(*this);
}

std::unique_ptr<BoundExpression> BoundBinaryExpression::clone() const
{
    return std::make_unique<BoundBinaryExpression>(
        operator_type(),
        left().clone(),
        right().clone(),
        logical_type()
    );
}

} // namespace dreamdb::binder::bound
