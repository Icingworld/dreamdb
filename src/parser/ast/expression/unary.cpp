#include "dreamdb/parser/ast/expression/unary.h"

#include <cassert>

#include "dreamdb/parser/ast/expression/visitor.h"

namespace dreamdb::parser::ast
{

AstUnaryExpression::AstUnaryExpression(
    AstUnaryOperatorType unary_type,
    std::unique_ptr<AstExpression> operand,
    std::size_t line,
    std::size_t column
)
    : AstExpression(AstExpressionType::Unary, line, column)
    , unary_type_(unary_type)
    , operand_(std::move(operand))
{
    // 操作数不能为空
    assert(operand_ != nullptr);
}

std::unique_ptr<AstUnaryExpression> AstUnaryExpression::create(
    AstUnaryOperatorType unary_type,
    std::unique_ptr<AstExpression> operand,
    std::size_t line,
    std::size_t column
)
{
    return std::make_unique<AstUnaryExpression>(
        unary_type, std::move(operand), line, column
    );
}

AstUnaryOperatorType AstUnaryExpression::unary_type() const noexcept
{
    return unary_type_;
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
