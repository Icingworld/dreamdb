#include "dreamdb/parser/ast/expression/expression.h"

namespace dreamdb::parser::ast
{

AstExpression::AstExpression(AstExpressionType expression_type, std::size_t line, std::size_t column) noexcept
    : AstNode(line, column)
    , expression_type_(expression_type)
{
}

AstExpressionType AstExpression::expression_type() const noexcept
{
    return expression_type_;
}

} // namespace dreamdb::parser::ast
