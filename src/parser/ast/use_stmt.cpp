#include "dreamdb/parser/ast/use_stmt.h"

#include <sstream>

namespace dreamdb
{

UseStmt::UseStmt(const std::string & database_name, std::size_t line, std::size_t column)
    : AstNode(AstNodeType::USE_STMT, line, column)
    , database_name_(database_name)
{
}

void UseStmt::set_database_name(const std::string & name)
{
    database_name_ = name;
}

const std::string & UseStmt::get_database_name() const noexcept
{
    return database_name_;
}

std::string UseStmt::debug_string() const
{
    std::ostringstream oss;
    oss << "UseStmt(database=" << (database_name_.empty() ? "<none>" : database_name_) << ")";
    return oss.str();
}

} // namespace dreamdb
