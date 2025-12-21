#include "dreamdb/parser/ast/insert_stmt.h"

#include <sstream>

namespace dreamdb
{

InsertStmt::InsertStmt(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::INSERT_STMT, line, column)
    , collection_name_("")
    , column_names_()
    , values_()
{
}

void InsertStmt::set_collection_name(const std::string & collection_name)
{
    collection_name_ = collection_name;
}

void InsertStmt::add_column_name(const std::string & column)
{
    column_names_.push_back(column);
}

void InsertStmt::add_value(std::unique_ptr<AstNode> value)
{
    values_.push_back(std::move(value));
}

const std::string & InsertStmt::get_collection_name() const noexcept
{
    return collection_name_;
}

const std::vector<std::string> & InsertStmt::get_column_names() const noexcept
{
    return column_names_;
}

const std::vector<std::unique_ptr<AstNode>> & InsertStmt::get_values() const noexcept
{
    return values_;
}

std::string InsertStmt::debug_string() const
{
    std::ostringstream oss;
    oss << "InsertStmt(collection_name=" << (collection_name_.empty() ? "<none>" : collection_name_);

    if (!column_names_.empty()) {
        oss << ", columns=[";
        for (std::size_t i = 0; i < column_names_.size(); ++i) {
            if (i > 0) {
                oss << ", ";
            }
            oss << column_names_[i];
        }
        oss << "]";
    } else {
        oss << ", columns=<SCHEMA_ORDER>";
    }

    oss << ", values=[";
    for (std::size_t i = 0; i < values_.size(); ++i) {
        if (i > 0) {
            oss << ", ";
        }
        oss << values_[i]->debug_string();
    }
    oss << "]";

    return oss.str();
}

} // namespace dreamdb
