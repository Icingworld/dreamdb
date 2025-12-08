#include "dreamdb/parser/ast/use_stmt.h"

#include <sstream>

namespace dreamdb
{

UseStmt::UseStmt(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::USE_STMT, line, column)
{
}

void UseStmt::set_database_name(const std::string & name)
{
    database_name = name;
}

const std::string & UseStmt::get_database_name() const noexcept
{
    return database_name;
}

std::string UseStmt::debug_string() const
{
    std::ostringstream oss;
    oss << "UseStmt(database=" << (database_name.empty() ? "<none>" : database_name) << ")";
    return oss.str();
}

} // namespace dreamdb

