#include "dreamdb/parser/ast/show_stmt.h"

#include <sstream>

namespace dreamdb
{

ShowStmt::ShowStmt(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::SHOW_STMT, line, column)
    , show_type_(ShowType::DATABASES)
    , collection_name_("")
{
}

void ShowStmt::set_show_type(ShowType show_type) noexcept
{
    show_type_ = show_type;
}

void ShowStmt::set_collection_name(const std::string & collection_name)
{
    collection_name_ = collection_name;
}

ShowStmt::ShowType ShowStmt::get_show_type() const noexcept
{
    return show_type_;
}

const std::string & ShowStmt::get_collection_name() const noexcept
{
    return collection_name_;
}

std::string ShowStmt::debug_string() const
{
    std::ostringstream oss;
    oss << "ShowStmt(";

    switch (show_type_) {
        case ShowType::DATABASES:
            oss << "DATABASES";
            break;
        case ShowType::COLLECTIONS:
            oss << "COLLECTIONS";
            break;
        case ShowType::INDEXES:
            oss << "INDEXES";
            break;
        default:
            oss << "UNKNOWN";
            break;
    }

    oss << ", name=" << (collection_name_.empty() ? "<none>" : collection_name_);
    oss << ")";

    return oss.str();
}

} // namespace dreamdb
