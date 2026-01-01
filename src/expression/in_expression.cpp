#include "dreamdb/expression/in_expression.h"

namespace dreamdb
{

InExpression::InExpression(std::unique_ptr<Expression> value, std::vector<std::unique_ptr<Expression>> list, bool negated)
    : Expression(ExpressionType::EXPRESSION_IN)
    , value_(std::move(value))
    , list_(std::move(list))
    , negated_(negated)
{
}

const Expression & InExpression::get_value() const noexcept
{
    return *value_;
}

Expression & InExpression::get_mutable_value() noexcept
{
    return *value_;
}

const std::vector<std::unique_ptr<Expression>> & InExpression::get_list() const noexcept
{
    return list_;
}

std::vector<std::unique_ptr<Expression>> & InExpression::get_mutable_list() noexcept
{
    return list_;
}

bool InExpression::is_negated() const noexcept
{
    return negated_;
}

} // namespace dreamdb
