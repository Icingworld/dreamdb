#include "dreamdb/expression/binary_expression.h"

namespace dreamdb
{

BinaryExpression::BinaryExpression(BinaryOperatorType operator_type, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
    : Expression(ExpressionType::EXPRESSION_BINARY)
    , operator_type_(operator_type)
    , left_(std::move(left))
    , right_(std::move(right))
{
}

BinaryOperatorType BinaryExpression::get_operator_type() const noexcept
{
    return operator_type_;
}

const Expression & BinaryExpression::get_left() const noexcept
{
    return *left_;
}

const Expression & BinaryExpression::get_right() const noexcept
{
    return *right_;
}

Expression & BinaryExpression::get_mutable_left() noexcept
{
    return *left_;
}

Expression & BinaryExpression::get_mutable_right() noexcept
{
    return *right_;
}

} // namespace dreamdb
