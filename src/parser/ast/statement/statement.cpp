#include "dreamdb/parser/ast/statement/statement.h"

namespace dreamdb::parser::ast
{

AstStatement::AstStatement(AstStatementType statement_type, std::size_t line, std::size_t column) noexcept
    : AstNode(line, column)
    , statement_type_(statement_type)
{
}

AstStatementType AstStatement::statement_type() const noexcept
{
    return statement_type_;
}

} // namespace dreamdb::parser::ast
