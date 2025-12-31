#include "dreamdb/expression/like_expression.h"

namespace dreamdb
{

LikeExpression::LikeExpression(std::unique_ptr<Expression> value, std::unique_ptr<Expression> pattern, bool negated)
    : Expression(ExpressionType::EXPRESSION_LIKE)
    , value_(std::move(value))
    , pattern_(std::move(pattern))
    , negated_(negated)
{
}

const Expression & LikeExpression::get_value() const noexcept
{
    return *value_;
}

Expression & LikeExpression::get_mutable_value() noexcept
{
    return *value_;
}

const Expression & LikeExpression::get_pattern() const noexcept
{
    return *pattern_;
}

Expression & LikeExpression::get_mutable_pattern() noexcept
{
    return *pattern_;
}

bool LikeExpression::is_negated() const noexcept
{
    return negated_;
}

} // namespace dreamdb
