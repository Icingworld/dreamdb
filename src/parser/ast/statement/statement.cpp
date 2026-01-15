#include "dreamdb/parser/ast/statement/statement.h"

namespace dreamdb::parser::ast
{

AstStatement::AstStatement(std::size_t line, std::size_t column) noexcept
    : AstNode(line, column)
{
}

} // namespace dreamdb::parser::ast
