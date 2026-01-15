#include "dreamdb/parser/ast/expression/expression.h"

namespace dreamdb::parser::ast
{

AstExpression::AstExpression(std::size_t line, std::size_t column) noexcept
    : AstNode(line, column)
{
}

} // namespace dreamdb::parser::ast
