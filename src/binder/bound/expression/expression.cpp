#include "dreamdb/binder/bound/expression/expression.h"

namespace dreamdb::binder::bound
{

BoundExpression::BoundExpression(BoundExpressionType expression_type, dreamdb::common::LogicalType logical_type)
    : expression_type_(expression_type)
    , logical_type_(logical_type)
{
}

BoundExpressionType BoundExpression::expression_type() const noexcept
{
    return expression_type_;
}

dreamdb::common::LogicalType BoundExpression::logical_type() const noexcept
{
    return logical_type_;
}

} // namespace dreamdb::binder::bound
