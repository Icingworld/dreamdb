#include "dreamdb/expression/function_expression.h"

namespace dreamdb
{

FunctionExpression::FunctionExpression(const std::string & function_name, std::vector<std::unique_ptr<Expression>> arguments, bool is_aggregate)
    : Expression(ExpressionType::EXPRESSION_FUNCTION)
    , function_name_(function_name)
    , arguments_(std::move(arguments))
    , is_aggregate_(is_aggregate)
{
}

const std::string & FunctionExpression::get_function_name() const noexcept
{
    return function_name_;
}

const std::vector<std::unique_ptr<Expression>> & FunctionExpression::get_arguments() const noexcept
{
    return arguments_;
}

std::vector<std::unique_ptr<Expression>> & FunctionExpression::get_mutable_arguments() noexcept
{
    return arguments_;
}

bool FunctionExpression::is_aggregate() const noexcept
{
    return is_aggregate_;
}

} // namespace dreamdb
