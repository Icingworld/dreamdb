#include "dreamdb/parser/ast/create_stmt.h"

#include <sstream>

namespace dreamdb
{

ColumnDefinition::ColumnDefinition()
    : name("")
    , type(FieldType::INT64)
    , length(0)
    , precision(0)
    , nullable(true)
    , primary_key(false)
    , auto_increment(false)
    , default_value(nullptr)
{
}

void ColumnDefinition::set_name(const std::string & name)
{
    this->name = name;
}

const std::string & ColumnDefinition::get_name() const noexcept
{
    return name;
}

void ColumnDefinition::set_type(FieldType type)
{
    this->type = type;
}

FieldType ColumnDefinition::get_type() const noexcept
{
    return type;
}

void ColumnDefinition::set_length(int length)
{
    this->length = length;
}

int ColumnDefinition::get_length() const noexcept
{
    return length;
}

void ColumnDefinition::set_precision(int precision)
{
    this->precision = precision;
}

int ColumnDefinition::get_precision() const noexcept
{
    return precision;
}

void ColumnDefinition::set_nullable(bool nullable)
{
    this->nullable = nullable;
}

bool ColumnDefinition::is_nullable() const noexcept
{
    return nullable;
}

void ColumnDefinition::set_primary_key(bool primary_key)
{
    this->primary_key = primary_key;
}

bool ColumnDefinition::is_primary_key() const noexcept
{
    return primary_key;
}

void ColumnDefinition::set_auto_increment(bool auto_increment)
{
    this->auto_increment = auto_increment;
}

bool ColumnDefinition::is_auto_increment() const noexcept
{
    return auto_increment;
}

void ColumnDefinition::set_default_value(std::unique_ptr<AstNode> default_value)
{
    this->default_value = std::move(default_value);
}

const AstNode * ColumnDefinition::get_default_value() const noexcept
{
    return default_value.get();
}

bool ColumnDefinition::has_default_value() const noexcept
{
    return default_value != nullptr;
}

CreateStmt::CreateStmt(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::CREATE_STMT, line, column)
    , object_type(ObjectType::DATABASE)
    , object_name("")
    , column_definitions()
{
}

void CreateStmt::set_object_type(ObjectType type)
{
    object_type = type;
}

CreateStmt::ObjectType CreateStmt::get_object_type() const noexcept
{
    return object_type;
}

void CreateStmt::set_object_name(const std::string & name)
{
    object_name = name;
}

const std::string & CreateStmt::get_object_name() const noexcept
{
    return object_name;
}

void CreateStmt::add_column_definition(ColumnDefinition column)
{
    column_definitions.push_back(std::move(column));
}

const std::vector<ColumnDefinition> & CreateStmt::get_column_definitions() const noexcept
{
    return column_definitions;
}

std::size_t CreateStmt::get_column_count() const noexcept
{
    return column_definitions.size();
}

std::string CreateStmt::debug_string() const
{
    std::ostringstream oss;
    oss << "CreateStmt(";

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

    // 列定义
    if (!column_definitions.empty()) {
        oss << ", columns=[";
        for (std::size_t i = 0; i < column_definitions.size(); ++i) {
            if (i > 0) {
                oss << ", ";
            }
            const auto & col = column_definitions[i];
            oss << col.get_name() << " ";

            // 类型
            switch (col.get_type()) {
                case FieldType::INT8: oss << "INT8"; break;
                case FieldType::INT16: oss << "INT16"; break;
                case FieldType::INT32: oss << "INT32"; break;
                case FieldType::INT64: oss << "INT64"; break;
                case FieldType::FLOAT: oss << "FLOAT"; break;
                case FieldType::DOUBLE: oss << "DOUBLE"; break;
                case FieldType::CHAR: oss << "CHAR"; break;
                case FieldType::VARCHAR: oss << "VARCHAR"; break;
                case FieldType::BOOLEAN: oss << "BOOLEAN"; break;
                case FieldType::TIMESTAMP: oss << "TIMESTAMP"; break;
                case FieldType::ENUM: oss << "ENUM"; break;
                case FieldType::FLOAT_VECTOR: oss << "FLOAT_VECTOR"; break;
            }

            // 长度
            if (col.get_length() > 0) {
                oss << "(" << col.get_length() << ")";
            }

            // 属性
            if (col.is_primary_key()) {
                oss << " PRIMARY_KEY";
            }
            if (col.is_auto_increment()) {
                oss << " AUTO_INCREMENT";
            }
            if (!col.is_nullable()) {
                oss << " NOT_NULL";
            }
        }
        oss << "]";
    }

    oss << ")";
    return oss.str();
}

} // namespace dreamdb
