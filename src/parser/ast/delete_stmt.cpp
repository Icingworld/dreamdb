#include "dreamdb/parser/ast/delete_stmt.h"

#include <sstream>

namespace dreamdb
{

DeleteStmt::DeleteStmt(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::DELETE_STMT, line, column)
    , collection_name_("")
    , where_clause_(nullptr)
{
}

void DeleteStmt::set_collection_name(const std::string & collection_name)
{
    collection_name_ = collection_name;
}

const std::string & DeleteStmt::get_collection_name() const noexcept
{
    return collection_name_;
}

void DeleteStmt::set_where_clause(std::unique_ptr<AstNode> where_clause) noexcept
{
    where_clause_ = std::move(where_clause);
}

const AstNode * DeleteStmt::get_where_clause() const noexcept
{
    return where_clause_.get();
}

bool DeleteStmt::has_where_clause() const noexcept
{
    return where_clause_ != nullptr;
}

std::string DeleteStmt::debug_string() const
{
    std::ostringstream oss;
    oss << "DeleteStmt(collection_name=" << (collection_name_.empty() ? "<none>" : collection_name_);

    if (has_where_clause()) {
        oss << ", where_clause=" << where_clause_->debug_string();
    } else {
        oss << ", where_clause=<none>";
    }

    oss << ")";

    return oss.str();
}

} // namespace dreamdb
