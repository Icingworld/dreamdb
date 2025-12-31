#include "dreamdb/expression/constant_expression.h"

namespace dreamdb
{

ConstantExpression::ConstantExpression(const FieldValue & field_value)
    : Expression(ExpressionType::EXPRESSION_CONSTANT),
      field_value_(field_value)
{
}

const FieldValue & ConstantExpression::get_field_value() const noexcept
{
    return field_value_;
}

} // namespace dreamdb
