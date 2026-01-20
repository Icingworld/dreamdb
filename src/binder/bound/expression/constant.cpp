#include "dreamdb/binder/bound/expression/constant.h"

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

} // namespace dreamdb::binder::bound
