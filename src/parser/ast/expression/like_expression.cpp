#include "dreamdb/parser/ast/expression/like_expression.h"

#include <cassert>

#include "dreamdb/parser/ast/expression/expression_visitor.h"

namespace dreamdb::parser::ast
{

AstLikeExpression::AstLikeExpression(
    std::unique_ptr<AstExpression> left,
    std::unique_ptr<AstExpression> pattern,
    bool is_not,
    std::size_t line,
    std::size_t column
)
    : AstExpression(AstExpressionType::Like, line, column)
    , left_(std::move(left))
    , pattern_(std::move(pattern))
    , is_not_(is_not)
{
    // 左侧表达式不能为空
    assert(left_ != nullptr);

    // 模式表达式不能为空
    assert(pattern_ != nullptr);
}

std::unique_ptr<AstLikeExpression> AstLikeExpression::create(
    std::unique_ptr<AstExpression> left,
    std::unique_ptr<AstExpression> pattern,
    bool is_not,
    std::size_t line,
    std::size_t column
)
{
    return std::make_unique<AstLikeExpression>(
        std::move(left), std::move(pattern), is_not, line, column
    );
}

const AstExpression & AstLikeExpression::left() const noexcept
{
    return *left_;
}

const AstExpression & AstLikeExpression::pattern() const noexcept
{
    return *pattern_;
}

bool AstLikeExpression::is_not() const noexcept
{
    return is_not_;
}

void AstLikeExpression::accept(AstExpressionVisitor & visitor) const
{
    visitor.visit(*this);
}

} // namespace dreamdb::parser::ast
