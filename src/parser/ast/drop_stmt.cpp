#include "dreamdb/parser/ast/drop_stmt.h"

#include <sstream>

namespace dreamdb
{

DropStmt::DropStmt(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::DROP_STMT, line, column)
    , object_type(ObjectType::DATABASE)
{
}

void DropStmt::set_object_type(ObjectType type)
{
    object_type = type;
}

DropStmt::ObjectType DropStmt::get_object_type() const noexcept
{
    return object_type;
}

void DropStmt::set_object_name(const std::string & name)
{
    object_name = name;
}

const std::string & DropStmt::get_object_name() const noexcept
{
    return object_name;
}

std::string DropStmt::debug_string() const
{
    std::ostringstream oss;
    oss << "DropStmt(";

    // 对象类型
    switch (object_type) {
        case ObjectType::DATABASE:
            oss << "DATABASE";
            break;
        case ObjectType::COLLECTION:
            oss << "COLLECTION";
            break;
        case ObjectType::INDEX:
            oss << "INDEX";
            break;
        default:
            oss << "UNKNOWN";
            break;
    }

    oss << ", name=" << (object_name.empty() ? "<none>" : object_name);
    oss << ")";
    return oss.str();
}

} // namespace dreamdb
