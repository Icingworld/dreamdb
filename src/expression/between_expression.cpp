#include "dreamdb/expression/between_expression.h"

namespace dreamdb
{

BetweenExpression::BetweenExpression(std::unique_ptr<Expression> value, std::unique_ptr<Expression> min, std::unique_ptr<Expression> max, bool negated)
    : Expression(ExpressionType::EXPRESSION_BETWEEN)
    , value_(std::move(value))
    , min_(std::move(min))
    , max_(std::move(max))
    , negated_(negated)
{
}

const Expression & BetweenExpression::get_value() const noexcept
{
    return *value_;
}

Expression & BetweenExpression::get_mutable_value() noexcept
{
    return *value_;
}

const Expression & BetweenExpression::get_min() const noexcept
{
    return *min_;
}

Expression & BetweenExpression::get_mutable_min() noexcept
{
    return *min_;
}

const Expression & BetweenExpression::get_max() const noexcept
{
    return *max_;
}

Expression & BetweenExpression::get_mutable_max() noexcept
{
    return *max_;
}

bool BetweenExpression::is_negated() const noexcept
{
    return negated_;
}

} // namespace dreamdb
