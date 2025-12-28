#include "dreamdb/parser/ast/delete_stmt.h"

#include <sstream>

namespace dreamdb
{

DeleteStmt::DeleteStmt(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::DELETE_STMT, line, column)
    , collection_name_("")
    , where_clause_(nullptr)
    , order_column_(std::nullopt)
    , order_type_(std::nullopt)
    , limit_(std::nullopt)
{
}

void DeleteStmt::set_collection_name(const std::string & collection_name)
{
    collection_name_ = collection_name;
}

void DeleteStmt::set_where_clause(std::unique_ptr<AstNode> where_clause) noexcept
{
    where_clause_ = std::move(where_clause);
}

void DeleteStmt::set_order_column(const std::string & order_column) noexcept
{
    order_column_ = order_column;
}

void DeleteStmt::set_order_type(Direction order_type) noexcept
{
    order_type_ = order_type;
}

void DeleteStmt::set_limit(std::size_t limit) noexcept
{
    limit_ = limit;
}

const std::string & DeleteStmt::get_collection_name() const noexcept
{
    return collection_name_;
}

const AstNode * DeleteStmt::get_where_clause() const noexcept
{
    return where_clause_.get();
}

const std::string & DeleteStmt::get_order_column() const noexcept
{
    return order_column_.value();
}

Direction DeleteStmt::get_order_type() const noexcept
{
    return order_type_.value();
}

std::size_t DeleteStmt::get_limit() const noexcept
{
    return limit_.value();
}

std::string DeleteStmt::debug_string() const
{
    std::ostringstream oss;
    oss << "DeleteStmt(collection_name=" << (collection_name_.empty() ? "<none>" : collection_name_);

    if (where_clause_ != nullptr) {
        oss << ", where_clause=" << where_clause_->debug_string();
    } else {
        oss << ", where_clause=<none>";
    }

    if (order_column_ != std::nullopt) {
        oss << ", order_column=" << order_column_.value();
    } else {
        oss << ", order_column=<none>";
    }

    if (order_type_ != std::nullopt) {
        oss << ", order_type=" << (order_type_.value() == Direction::ASC ? "ASC" : "DESC");
    } else {
        oss << ", order_type=<none>";
    }

    if (limit_ != std::nullopt) {
        oss << ", limit=" << limit_.value();
    } else {
        oss << ", limit=<none>";
    }

    oss << ")";

    return oss.str();
}

} // namespace dreamdb
