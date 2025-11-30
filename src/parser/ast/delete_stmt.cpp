#include "dreamdb/parser/ast/delete_stmt.h"

#include <sstream>

namespace dreamdb
{

DeleteStmt::DeleteStmt(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::DELETE_STMT, line, column)
    , table_name("")
    , where_clause(nullptr)
{
}

void DeleteStmt::set_table_name(const std::string & name)
{
    table_name = name;
}

const std::string & DeleteStmt::get_table_name() const
{
    return table_name;
}

void DeleteStmt::set_where_clause(std::unique_ptr<AstNode> where)
{
    where_clause = std::move(where);
}

const AstNode * DeleteStmt::get_where_clause() const
{
    return where_clause.get();
}

bool DeleteStmt::has_where_clause() const
{
    return where_clause != nullptr;
}

std::string DeleteStmt::debug_string() const
{
    std::ostringstream oss;
    oss << "DeleteStmt(table=" << (table_name.empty() ? "<none>" : table_name);

    if (has_where_clause()) {
        oss << ", where=" << where_clause->debug_string();
    } else {
        oss << ", where=<none>";
    }

    oss << ")";

    return oss.str();
}

} // namespace dreamdb
