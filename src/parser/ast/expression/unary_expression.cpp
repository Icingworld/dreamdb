#include "dreamdb/parser/ast/expression/unary_expression.h"

#include <cassert>

#include "dreamdb/parser/ast/expression/expression_visitor.h"

namespace dreamdb::parser::ast
{

AstUnaryExpression::AstUnaryExpression(
    AstUnaryOperatorType type,
    std::unique_ptr<AstExpression> operand,
    std::size_t line,
    std::size_t column
)
    : AstExpression(line, column)
    , type_(type)
    , operand_(std::move(operand))
{
    // 操作数不能为空
    assert(operand_ != nullptr);
}

std::unique_ptr<AstUnaryExpression> AstUnaryExpression::create(
    AstUnaryOperatorType type,
    std::unique_ptr<AstExpression> operand,
    std::size_t line,
    std::size_t column
)
{
    return std::make_unique<AstUnaryExpression>(
        type, std::move(operand), line, column
    );
}

AstUnaryOperatorType AstUnaryExpression::type() const noexcept
{
    return type_;
}

const AstExpression & AstUnaryExpression::operand() const noexcept
{
    return *operand_;
}

void AstUnaryExpression::accept(AstExpressionVisitor & visitor) const
{
    visitor.visit(*this);
}

} // namespace dreamdb::parser::ast
