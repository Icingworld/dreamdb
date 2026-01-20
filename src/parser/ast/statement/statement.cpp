#include "dreamdb/parser/ast/statement/statement.h"

namespace dreamdb::parser::ast
{

AstStatement::AstStatement(AstStatementType type, std::size_t line, std::size_t column) noexcept
    : AstNode(line, column)
    , type_(type)
{
}

AstStatementType AstStatement::type() const noexcept
{
    return type_;
}

} // namespace dreamdb::parser::ast
