#include "dreamdb/parser/ast/expression/expression.h"

namespace dreamdb::parser::ast
{

AstExpression::AstExpression(AstExpressionType type, std::size_t line, std::size_t column) noexcept
    : AstNode(line, column)
    , type_(type)
{
}

AstExpressionType AstExpression::type() const noexcept
{
    return type_;
}

} // namespace dreamdb::parser::ast
