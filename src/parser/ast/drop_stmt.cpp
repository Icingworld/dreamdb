#include "dreamdb/parser/ast/drop_stmt.h"

#include <sstream>

namespace dreamdb
{

std::string DropStmt::drop_type_to_string(DropType drop_type)
{
    switch (drop_type) {
        case DropType::DATABASE:
            return "DATABASE";
        case DropType::COLLECTION:
            return "COLLECTION";
        case DropType::INDEX:
            return "INDEX";
        case DropType::VINDEX:
            return "VINDEX";
        default:
            return "UNKNOWN";
    }
}

DropStmt::DropStmt(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::DROP_STMT, line, column)
    , drop_type_(DropType::DATABASE)
    , object_name_("")
    , collection_name_("")
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

void DropStmt::set_collection_name(const std::string & collection_name)
{
    collection_name_ = collection_name;
}

const std::string & DropStmt::get_collection_name() const noexcept
{
    return collection_name_;
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
        case DropType::VINDEX:
            oss << "VINDEX";
            break;
        default:
            oss << "UNKNOWN";
            break;
    }

    oss << ", object_name=" << (object_name_.empty() ? "<none>" : object_name_);
    
    // 如果是 INDEX 或 VINDEX，显示集合名称
    if (drop_type_ == DropType::INDEX || drop_type_ == DropType::VINDEX) {
        oss << ", collection_name=" << (collection_name_.empty() ? "<none>" : collection_name_);
    }
    
    oss << ")";

    return oss.str();
}

} // namespace dreamdb
