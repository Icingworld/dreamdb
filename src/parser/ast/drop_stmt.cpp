#include "dreamdb/parser/ast/drop_stmt.h"

#include <sstream>

namespace dreamdb
{

DropStmt::DropStmt(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::DROP_STMT, line, column)
    , drop_type_(DropType::DATABASE)
    , object_name_("")
{
}

void DropStmt::set_drop_type(DropType drop_type)
{
    drop_type_ = drop_type;
}

DropStmt::DropType DropStmt::get_drop_type() const noexcept
{
    return drop_type_;
}

void DropStmt::set_object_name(const std::string & object_name)
{
    object_name_ = object_name;
}

const std::string & DropStmt::get_object_name() const noexcept
{
    return object_name_;
}

std::string DropStmt::debug_string() const
{
    std::ostringstream oss;
    oss << "DropStmt(";

    // 删除类型
    switch (drop_type_) {
        case DropType::DATABASE:
            oss << "DATABASE";
            break;
        case DropType::COLLECTION:
            oss << "COLLECTION";
            break;
        case DropType::INDEX:
            oss << "INDEX";
            break;
        default:
            oss << "UNKNOWN";
            break;
    }

    oss << ", object_name=" << (object_name_.empty() ? "<none>" : object_name_);
    oss << ")";

    return oss.str();
}

} // namespace dreamdb
