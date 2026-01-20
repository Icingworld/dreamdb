#include "dreamdb/parser/ast/expression/function_call.h"

#include <cassert>

#include "dreamdb/parser/ast/expression/expression_visitor.h"

namespace dreamdb::parser::ast
{

AstFunctionCallExpression::AstFunctionCallExpression(
    std::string function_name,
    std::vector<std::unique_ptr<AstExpression>> arguments,
    std::size_t line,
    std::size_t column
)
    : AstExpression(line, column)
    , function_name_(std::move(function_name))
    , arguments_(std::move(arguments))
{
    // 函数名不能为空
    assert(!function_name_.empty());

    // 参数列表不能为空
    assert(!arguments_.empty());

    // 参数列表中的每个参数都不能为空
    for (const auto & argument : arguments_) {
        assert(argument != nullptr);
    }
}

std::unique_ptr<AstFunctionCallExpression> AstFunctionCallExpression::create(
    std::string function_name,
    std::vector<std::unique_ptr<AstExpression>> arguments,
    std::size_t line,
    std::size_t column
)
{
    return std::make_unique<AstFunctionCallExpression>(
        std::move(function_name), std::move(arguments), line, column
    );
}

const std::string & AstFunctionCallExpression::function_name() const noexcept
{
    return function_name_;
}

const AstExpression & AstFunctionCallExpression::argument_at(std::size_t index) const noexcept
{
    // 索引不能超出范围
    assert(index < arguments_.size());

    return *arguments_[index];
}

std::size_t AstFunctionCallExpression::argument_count() const noexcept
{
    return arguments_.size();
}

void AstFunctionCallExpression::accept(AstExpressionVisitor & visitor) const
{
    visitor.visit(*this);
}

} // namespace dreamdb::parser::ast
