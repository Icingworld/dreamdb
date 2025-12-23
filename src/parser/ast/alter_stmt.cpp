#include "dreamdb/parser/ast/alter_stmt.h"

#include <sstream>

namespace dreamdb
{

AlterStmt::AlterStmt(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::ALTER_STMT, line, column)
    , collection_name_("")
    , type_(AlterType::ADD_COLUMN)
    , column_name_("")
    , old_column_name_(std::nullopt)
    , new_column_definition_(std::nullopt)
{
}

void AlterStmt::set_collection_name(const std::string & collection_name)
{
    collection_name_ = collection_name;
}

void AlterStmt::set_alter_type(AlterType type) noexcept
{
    type_ = type;
}

void AlterStmt::set_column_name(const std::string & column_name)
{
    column_name_ = column_name;
}

void AlterStmt::set_old_column_name(const std::string & old_column_name)
{
    old_column_name_ = old_column_name;
}

void AlterStmt::set_new_column_definition(ColumnDefinition && new_column_definition) noexcept
{
    new_column_definition_ = std::move(new_column_definition);
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

const std::optional<std::string> & AlterStmt::get_old_column_name() const noexcept
{
    return old_column_name_;
}

const std::optional<ColumnDefinition> & AlterStmt::get_new_column_definition() const noexcept
{
    return new_column_definition_;
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

    // 根据操作类型输出相应信息
    switch (type_) {
        case AlterType::ADD_COLUMN:
            // ADD_COLUMN: 输出新字段定义
            if (new_column_definition_) {
                const ColumnDefinition & col = *new_column_definition_;
                oss << ", column_name=" << col.get_name() << ", type=";
                
                // 输出字段类型
                switch (col.get_type()) {
                    case FieldType::TINYINT:
                        oss << "TINYINT";
                        break;
                    case FieldType::SMALLINT:
                        oss << "SMALLINT";
                        break;
                    case FieldType::INTEGER:
                        oss << "INTEGER";
                        break;
                    case FieldType::BIGINT:
                        oss << "BIGINT";
                        break;
                    case FieldType::FLOAT:
                        oss << "FLOAT";
                        break;
                    case FieldType::DOUBLE:
                        oss << "DOUBLE";
                        break;
                    case FieldType::DECIMAL:
                        oss << "DECIMAL";
                        break;
                    case FieldType::CHAR:
                        oss << "CHAR";
                        break;
                    case FieldType::VARCHAR:
                        oss << "VARCHAR";
                        break;
                    case FieldType::BOOLEAN:
                        oss << "BOOLEAN";
                        break;
                    case FieldType::TIMESTAMP:
                        oss << "TIMESTAMP";
                        break;
                    case FieldType::ENUM:
                        oss << "ENUM";
                        break;
                    case FieldType::VECTOR:
                        oss << "VECTOR";
                        break;
                    default:
                        oss << "UNKNOWN";
                        break;
                }

                // 输出长度或精度
                if (col.get_type() == FieldType::DECIMAL) {
                    // DECIMAL 类型：显示 DECIMAL(p, s) 格式
                    if (col.get_length() > 0 || col.get_precision() > 0) {
                        oss << "(" << col.get_length();
                        if (col.get_precision() > 0) {
                            oss << ", " << col.get_precision();
                        }
                        oss << ")";
                    }
                } else if (col.get_length() > 0) {
                    // 其他类型：只显示长度
                    oss << "(" << col.get_length() << ")";
                }

                // 输出属性
                if (col.get_is_primary()) {
                    oss << " PRIMARY_KEY";
                }
                if (col.get_is_auto_increment()) {
                    oss << " AUTO_INCREMENT";
                }
                if (!col.get_is_nullable()) {
                    oss << " NOT_NULL";
                }
            } else {
                oss << ", column_definition=<none>";
            }
            break;

        case AlterType::DROP_COLUMN:
            // DROP_COLUMN: 只需要字段名
            oss << ", old_column_name=" << (old_column_name_.has_value() ? *old_column_name_ : "<none>");
            break;

        case AlterType::MODIFY_COLUMN:
            // MODIFY_COLUMN: 输出旧字段名和新字段定义
            oss << ", old_column_name=" << (old_column_name_.has_value() ? *old_column_name_ : "<none>");
            if (new_column_definition_) {
                const ColumnDefinition & col = *new_column_definition_;
                oss << ", new_type=";

                // 输出字段类型
                switch (col.get_type()) {
                    case FieldType::TINYINT:
                        oss << "TINYINT";
                        break;
                    case FieldType::SMALLINT:
                        oss << "SMALLINT";
                        break;
                    case FieldType::INTEGER:
                        oss << "INTEGER";
                        break;
                    case FieldType::BIGINT:
                        oss << "BIGINT";
                        break;
                    case FieldType::FLOAT:
                        oss << "FLOAT";
                        break;
                    case FieldType::DOUBLE:
                        oss << "DOUBLE";
                        break;
                    case FieldType::DECIMAL:
                        oss << "DECIMAL";
                        break;
                    case FieldType::CHAR:
                        oss << "CHAR";
                        break;
                    case FieldType::VARCHAR:
                        oss << "VARCHAR";
                        break;
                    case FieldType::BOOLEAN:
                        oss << "BOOLEAN";
                        break;
                    case FieldType::TIMESTAMP:
                        oss << "TIMESTAMP";
                        break;
                    case FieldType::ENUM:
                        oss << "ENUM";
                        break;
                    case FieldType::VECTOR:
                        oss << "VECTOR";
                        break;
                    default:
                        oss << "UNKNOWN";
                        break;
                }

                // 输出长度或精度
                if (col.get_type() == FieldType::DECIMAL) {
                    // DECIMAL 类型：显示 DECIMAL(p, s) 格式
                    if (col.get_length() > 0 || col.get_precision() > 0) {
                        oss << "(" << col.get_length();
                        if (col.get_precision() > 0) {
                            oss << ", " << col.get_precision();
                        }
                        oss << ")";
                    }
                } else if (col.get_length() > 0) {
                    // 其他类型：只显示长度
                    oss << "(" << col.get_length() << ")";
                }

                // 输出属性
                if (col.get_is_primary()) {
                    oss << " PRIMARY_KEY";
                }
                if (col.get_is_auto_increment()) {
                    oss << " AUTO_INCREMENT";
                }
                if (!col.get_is_nullable()) {
                    oss << " NOT_NULL";
                }
            } else {
                oss << ", new_type=<none>";
            }
            break;

        case AlterType::RENAME_COLUMN:
            // RENAME_COLUMN: 输出旧字段名和新字段名
            if (old_column_name_) {
                oss << ", old_column_name=" << *old_column_name_;
            } else {
                oss << ", old_column_name=<none>";
            }
            oss << ", new_column_name=" << (column_name_.empty() ? "<none>" : column_name_);
            break;

        default:
            break;
    }

    oss << ")";

    return oss.str();
}

} // namespace dreamdb
