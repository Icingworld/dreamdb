#include "dreamdb/binder/bound/expression/unary.h"

#include <cassert>

namespace dreamdb::binder::bound
{

BoundUnaryExpression::BoundUnaryExpression(
    BoundUnaryOperatorType operator_type,
    std::unique_ptr<BoundExpression> operand,
    dreamdb::common::LogicalType logical_type
)
    : BoundExpression(BoundExpressionType::Unary, logical_type)
    , operator_type_(operator_type)
    , operand_(std::move(operand))
{
    // 操作数不能为空
    assert(operand_ != nullptr);
}

BoundUnaryOperatorType BoundUnaryExpression::operator_type() const noexcept
{
    return operator_type_;
}

const BoundExpression & BoundUnaryExpression::operand() const noexcept
{
    return *operand_;
}

} // namespace dreamdb::binder::bound
