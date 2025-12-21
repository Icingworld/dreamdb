#include "dreamdb/parser/ast/update_stmt.h"

#include <sstream>

namespace dreamdb
{

UpdateStmt::UpdateStmt(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::UPDATE_STMT, line, column)
    , collection_name_("")
    , assignments_()
    , where_clause_(nullptr)
{
}

void UpdateStmt::set_collection_name(const std::string & collection_name)
{
    collection_name_ = collection_name;
}

void UpdateStmt::add_assignment(const std::string & column_name, std::unique_ptr<AstNode> value)
{
    assignments_.emplace_back(column_name, std::move(value));
}

const std::string & UpdateStmt::get_collection_name() const noexcept
{
    return collection_name_;
}

void UpdateStmt::set_where_clause(std::unique_ptr<AstNode> where_clause)
{
    where_clause_ = std::move(where_clause);
}

const std::vector<std::pair<std::string, std::unique_ptr<AstNode>>> & UpdateStmt::get_assignments() const noexcept
{
    return assignments_;
}

const AstNode * UpdateStmt::get_where_clause() const noexcept
{
    return where_clause_.get();
}

std::string UpdateStmt::debug_string() const
{
    std::ostringstream oss;
    oss << "UpdateStmt(collection_name=" << (collection_name_.empty() ? "<none>" : collection_name_);

    oss << ", set=[";
    for (std::size_t i = 0; i < assignments_.size(); ++i) {
        if (i > 0) {
            oss << ", ";
        }
        oss << assignments_[i].first << "=";
        if (assignments_[i].second) {
            oss << assignments_[i].second->debug_string();
        } else {
            oss << "<null>";
        }
    }
    oss << "]";

    if (where_clause_ != nullptr) {
        oss << ", where=" << where_clause_->debug_string();
    } else {
        oss << ", where=<none>";
    }

    oss << ")";

    return oss.str();
}

} // namespace dreamdb
