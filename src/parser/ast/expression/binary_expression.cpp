#include "dreamdb/parser/ast/expression/binary_expression.h"

#include <cassert>

#include "dreamdb/parser/ast/expression/expression_visitor.h"

namespace dreamdb::parser::ast
{

AstBinaryExpression::AstBinaryExpression(
    AstBinaryOperatorType type,
    std::unique_ptr<AstExpression> left,
    std::unique_ptr<AstExpression> right,
    std::size_t line,
    std::size_t column
)
    : AstExpression(line, column)
    , type_(type)
    , left_(std::move(left))
    , right_(std::move(right))
{
    // 左操作数不能为空
    assert(left_ != nullptr);

    // 右操作数不能为空
    assert(right_ != nullptr);
}

std::unique_ptr<AstBinaryExpression> AstBinaryExpression::create(
    AstBinaryOperatorType type,
    std::unique_ptr<AstExpression> left,
    std::unique_ptr<AstExpression> right,
    std::size_t line,
    std::size_t column
)
{
    return std::make_unique<AstBinaryExpression>(
        type, std::move(left), std::move(right), line, column
    );
}

AstBinaryOperatorType AstBinaryExpression::type() const noexcept
{
    return type_;
}

const AstExpression & AstBinaryExpression::left() const noexcept
{
    return *left_;
}

const AstExpression & AstBinaryExpression::right() const noexcept
{
    return *right_;
}

void AstBinaryExpression::accept(AstExpressionVisitor & visitor) const
{
    visitor.visit(*this);
}

} // namespace dreamdb::parser::ast
