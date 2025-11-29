#include "dreamdb/parser/ast/update_stmt.h"
#include "dreamdb/parser/ast/ast_node.h"

#include <sstream>

namespace dreamdb
{

UpdateStmt::UpdateStmt(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::UPDATE_STMT, line, column)
    , assignments()
    , where_clause(nullptr)
{
}

void UpdateStmt::set_table_name(const std::string & table)
{
    table_name = table;
}

const std::string & UpdateStmt::get_table_name() const noexcept
{
    return table_name;
}

void UpdateStmt::add_assignment(const std::string & column, std::unique_ptr<AstNode> value)
{
    assignments.emplace_back(column, std::move(value));
}

const std::vector<std::pair<std::string, std::unique_ptr<AstNode>>> & UpdateStmt::get_assignments() const noexcept
{
    return assignments;
}

std::size_t UpdateStmt::get_assignment_count() const noexcept
{
    return assignments.size();
}

void UpdateStmt::set_where_clause(std::unique_ptr<AstNode> expr)
{
    where_clause = std::move(expr);
}

const AstNode * UpdateStmt::get_where_clause() const noexcept
{
    return where_clause.get();
}

bool UpdateStmt::has_where_clause() const noexcept
{
    return where_clause != nullptr;
}

std::string UpdateStmt::debug_string() const
{
    std::ostringstream oss;
    oss << "UpdateStmt(table=" << (table_name.empty() ? "<none>" : table_name);

    oss << ", set=[";
    for (std::size_t i = 0; i < assignments.size(); ++i) {
        if (i > 0) {
            oss << ", ";
        }
        oss << assignments[i].first << "=";
        if (assignments[i].second) {
            oss << assignments[i].second->debug_string();
        } else {
            oss << "<null>";
        }
    }
    oss << "]";

    if (has_where_clause()) {
        oss << ", where=" << where_clause->debug_string();
    } else {
        oss << ", where=<none>";
    }

    oss << ")";
    return oss.str();
}

} // namespace dreamdb
