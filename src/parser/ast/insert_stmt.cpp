#include "dreamdb/parser/ast/insert_stmt.h"

#include <sstream>

namespace dreamdb
{

InsertStmt::InsertStmt(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::INSERT_STMT, line, column)
    , table_name("")
    , column_names()
    , values()
{
}

void InsertStmt::set_table_name(const std::string & table)
{
    table_name = table;
}

const std::string & InsertStmt::get_table_name() const noexcept
{
    return table_name;
}

void InsertStmt::add_column_name(const std::string & column)
{
    column_names.push_back(column);
}

const std::vector<std::string> & InsertStmt::get_column_names() const noexcept
{
    return column_names;
}

bool InsertStmt::has_column_names() const noexcept
{
    return !column_names.empty();
}

void InsertStmt::add_value(std::unique_ptr<AstNode> value)
{
    values.push_back(std::move(value));
}

const std::vector<std::unique_ptr<AstNode>> & InsertStmt::get_values() const noexcept
{
    return values;
}

std::string InsertStmt::debug_string() const
{
    std::ostringstream oss;
    oss << "InsertStmt(table=" << (table_name.empty() ? "<none>" : table_name);

    if (has_column_names()) {
        oss << ", columns=[";
        for (std::size_t i = 0; i < column_names.size(); ++i) {
            if (i > 0) {
                oss << ", ";
            }
            oss << column_names[i];
        }
        oss << "]";
    } else {
        oss << ", columns=<SCHEMA_ORDER>";
    }

    oss << ", values=[";
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i > 0) {
            oss << ", ";
        }
        oss << values[i]->debug_string();
    }
    oss << "]";

    return oss.str();
}

} // namespace dreamdb
