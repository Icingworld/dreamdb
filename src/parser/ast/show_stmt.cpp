#include "dreamdb/parser/ast/show_stmt.h"

#include <sstream>

namespace dreamdb
{

ShowStmt::ShowStmt(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::SHOW_STMT, line, column)
    , show_type_(ShowType::DATABASES)
    , object_name_("")
{
}

void ShowStmt::set_show_type(ShowType show_type) noexcept
{
    show_type_ = show_type;
}

void ShowStmt::set_object_name(const std::string & object_name)
{
    object_name_ = object_name;
}

ShowStmt::ShowType ShowStmt::get_show_type() const noexcept
{
    return show_type_;
}

const std::string & ShowStmt::get_object_name() const noexcept
{
    return object_name_;
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

    oss << ", name=" << (object_name_.empty() ? "<none>" : object_name_);
    oss << ")";

    return oss.str();
}

} // namespace dreamdb
