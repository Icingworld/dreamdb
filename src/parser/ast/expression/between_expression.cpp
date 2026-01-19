#include "dreamdb/parser/ast/expression/between_expression.h"

#include <cassert>

#include "dreamdb/parser/ast/expression/expression_visitor.h"

namespace dreamdb::parser::ast
{

AstBetweenExpression::AstBetweenExpression(
    std::unique_ptr<AstExpression> left,
    std::unique_ptr<AstExpression> start,
    std::unique_ptr<AstExpression> end,
    bool is_not,
    std::size_t line,
    std::size_t column
)
    : AstExpression(line, column)
    , left_(std::move(left))
    , start_(std::move(start))
    , end_(std::move(end))
    , is_not_(is_not)
{
    // 左侧表达式不能为空
    assert(left_ != nullptr);

    // 起始值表达式不能为空
    assert(start_ != nullptr);

    // 结束值表达式不能为空
    assert(end_ != nullptr);
}

std::unique_ptr<AstBetweenExpression> AstBetweenExpression::create(
    std::unique_ptr<AstExpression> left,
    std::unique_ptr<AstExpression> start,
    std::unique_ptr<AstExpression> end,
    bool is_not,
    std::size_t line,
    std::size_t column
)
{
    return std::make_unique<AstBetweenExpression>(
        std::move(left), std::move(start), std::move(end), is_not, line, column
    );
}

const AstExpression & AstBetweenExpression::left() const noexcept
{
    return *left_;
}

const AstExpression & AstBetweenExpression::start() const noexcept
{
    return *start_;
}

const AstExpression & AstBetweenExpression::end() const noexcept
{
    return *end_;
}

bool AstBetweenExpression::is_not() const noexcept
{
    return is_not_;
}

void AstBetweenExpression::accept(AstExpressionVisitor & visitor) const
{
    visitor.visit(*this);
}

} // namespace dreamdb::parser::ast
