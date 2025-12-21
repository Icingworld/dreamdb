#include "dreamdb/parser/ast/alter_stmt.h"

#include <sstream>

namespace dreamdb
{

AlterStmt::AlterStmt(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::ALTER_STMT, line, column)
    , collection_name_("")
    , type_(AlterType::ADD_COLUMN)
    , column_name_("")
    , new_column_name_(std::nullopt)
    , column_definition_(std::nullopt)
{
}

void AlterStmt::set_collection_name(const std::string & collection_name)
{
    collection_name_ = collection_name;
}

void AlterStmt::set_alter_type(AlterType type)
{
    type_ = type;
}

void AlterStmt::set_column_name(const std::string & column_name)
{
    column_name_ = column_name;
}

void AlterStmt::set_new_column_name(const std::string & new_column_name)
{
    new_column_name_ = new_column_name;
}

void AlterStmt::set_column_definition(ColumnDefinition && column_definition)
{
    column_definition_ = std::move(column_definition);
}

const std::string & AlterStmt::get_collection_name() const noexcept
{
    return collection_name_;
}

AlterStmt::AlterType AlterStmt::get_alter_type() const noexcept
{
    return type_;
}

const std::string & AlterStmt::get_column_name() const noexcept
{
    return column_name_;
}

const std::optional<std::string> & AlterStmt::get_new_column_name() const noexcept
{
    return new_column_name_;
}

const std::optional<ColumnDefinition> & AlterStmt::get_column_definition() const noexcept
{
    return column_definition_;
}

std::string AlterStmt::debug_string() const
{
    std::ostringstream oss;
    oss << "AlterStmt(collection_name=" << (collection_name_.empty() ? "<none>" : collection_name_)
        << ", operation=";

    // 输出操作类型
    switch (type_) {
        case AlterType::ADD_COLUMN:
            oss << "ADD_COLUMN";
            break;
        case AlterType::DROP_COLUMN:
            oss << "DROP_COLUMN";
            break;
        case AlterType::MODIFY_COLUMN:
            oss << "MODIFY_COLUMN";
            break;
        case AlterType::RENAME_COLUMN:
            oss << "RENAME_COLUMN";
            break;
        default:
            oss << "UNKNOWN";
            break;
    }

    // 输出字段名
    if (!column_name_.empty()) {
        oss << ", column_name=" << column_name_;
    }

    // 根据操作类型输出相应信息
    if (type_ == AlterType::RENAME_COLUMN && new_column_name_) {
        oss << ", new_column_name=" << *new_column_name_;
    }

    // ADD_COLUMN 和 MODIFY_COLUMN 需要显示列定义
    if ((type_ == AlterType::ADD_COLUMN || type_ == AlterType::MODIFY_COLUMN) && column_definition_) {
        const ColumnDefinition & col = *column_definition_;
        oss << ", column_definition=ColumnDefinition("
            << "name=" << col.get_name()
            << ", type=" << static_cast<int>(col.get_type());
        
        if (col.get_length() > 0) {
            oss << ", length=" << col.get_length();
        }
        if (col.get_precision() > 0) {
            oss << ", precision=" << col.get_precision();
        }
        oss << ")";
    }

    oss << ")";

    return oss.str();
}

} // namespace dreamdb
