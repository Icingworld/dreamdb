#include "dreamdb/binder/bound/expression/constant.h"

#include "dreamdb/binder/bound/expression/visitor.h"

namespace dreamdb::binder::bound
{

BoundConstantExpression::BoundConstantExpression(
    dreamdb::FieldValue field_value,
    dreamdb::common::LogicalType logical_type
)
    : BoundExpression(BoundExpressionType::Constant, logical_type)
    , field_value_(std::move(field_value))
{
}

const dreamdb::FieldValue & BoundConstantExpression::value() const noexcept
{
    return field_value_;
}

void BoundConstantExpression::accept(BoundExpressionVisitor & visitor) const
{
    visitor.visit(*this);
}

std::unique_ptr<BoundExpression> BoundConstantExpression::clone() const
{
    return std::make_unique<BoundConstantExpression>(
        value(),  // 复制 FieldValue
        logical_type()
    );
}

} // namespace dreamdb::binder::bound
