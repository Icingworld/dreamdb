#include "dreamdb/expression/null_expression.h"

namespace dreamdb
{

NullExpression::NullExpression(std::unique_ptr<Expression> value, bool negated)
    : Expression(ExpressionType::EXPRESSION_NULL)
    , value_(std::move(value))
    , negated_(negated)
{
}

const Expression & NullExpression::get_value() const noexcept
{
    return *value_;
}

Expression & NullExpression::get_mutable_value() noexcept
{
    return *value_;
}

bool NullExpression::is_negated() const noexcept
{
    return negated_;
}

} // namespace dreamdb
