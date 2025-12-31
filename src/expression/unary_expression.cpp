#include "dreamdb/expression/unary_expression.h"

namespace dreamdb
{

UnaryExpression::UnaryExpression(UnaryOperatorType operator_type, std::unique_ptr<Expression> operand)
    : Expression(ExpressionType::EXPRESSION_UNARY),
      operator_type_(operator_type),
      operand_(std::move(operand))
{
}

UnaryOperatorType UnaryExpression::get_operator_type() const noexcept
{
    return operator_type_;
}

const Expression & UnaryExpression::get_operand() const noexcept
{
    return *operand_;
}

Expression & UnaryExpression::get_mutable_operand() noexcept
{
    return *operand_;
}

} // namespace dreamdb
