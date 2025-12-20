#include "dreamdb/parser/ast/create_stmt.h"

#include <sstream>

namespace dreamdb
{

ColumnDefinition::ColumnDefinition()
    : name_("")
    , type_(FieldType::TINYINT)
    , length_(0)
    , precision_(0)
    , options_(std::nullopt)
    , is_nullable_(true)
    , is_primary_(false)
    , comment_("")
    , default_value_(nullptr)
    , is_auto_increment_(false)
{
}

void ColumnDefinition::set_name(const std::string & name)
{
    name_ = name;
}

void ColumnDefinition::set_type(FieldType type)
{
    type_ = type;
}

void ColumnDefinition::set_length(int length)
{
    length_ = length;
}

void ColumnDefinition::set_precision(int precision)
{
    precision_ = precision;
}

void ColumnDefinition::set_options(const std::vector<std::string> & options)
{
    options_ = options;
}

void ColumnDefinition::set_is_nullable(bool is_nullable)
{
    is_nullable_ = is_nullable;
}

void ColumnDefinition::set_is_primary(bool is_primary)
{
    is_primary_ = is_primary;
}

void ColumnDefinition::set_comment(const std::string & comment)
{
    comment_ = comment;
}

void ColumnDefinition::set_default_value(std::unique_ptr<AstNode> default_value)
{
    default_value_ = std::move(default_value);
}

void ColumnDefinition::set_is_auto_increment(bool is_auto_increment)
{
    is_auto_increment_ = is_auto_increment;
}

FieldType ColumnDefinition::get_type() const noexcept
{
    return type_;
}

const std::string & ColumnDefinition::get_name() const noexcept
{
    return name_;
}

int ColumnDefinition::get_length() const noexcept
{
    return length_;
}

int ColumnDefinition::get_precision() const noexcept
{
    return precision_;
}

const std::optional<std::vector<std::string>> & ColumnDefinition::get_options() const noexcept
{
    return options_;
}

bool ColumnDefinition::get_is_nullable() const noexcept
{
    return is_nullable_;
}

bool ColumnDefinition::get_is_primary() const noexcept
{
    return is_primary_;
}

const std::string & ColumnDefinition::get_comment() const noexcept
{
    return comment_;
}

const AstNode * ColumnDefinition::get_default_value() const noexcept
{
    return default_value_.get();
}

bool ColumnDefinition::get_is_auto_increment() const noexcept
{
    return is_auto_increment_;
}

CreateStmt::CreateStmt(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::CREATE_STMT, line, column)
    , create_type_(CreateType::DATABASE)   // 默认 DATABASE 不需要用到列定义和集合名称
    , object_name_("")
    , is_if_not_exists_(false)
    , column_definitions_(std::nullopt)
    , collection_name_(std::nullopt)
{
}

void CreateStmt::set_create_type(CreateType create_type) noexcept
{
    create_type_ = create_type;

    // 根据不同类型初始化 optional 成员
    switch (create_type_) {
        case CreateType::DATABASE:
            column_definitions_ = std::nullopt;
            collection_name_ = std::nullopt;
            break;
        case CreateType::COLLECTION:
            column_definitions_ = std::vector<ColumnDefinition>();
            collection_name_ = std::nullopt;    // 该集合名仅在 INDEX 中使用，COLLECTION 使用的是 object_name_
            break;
        case CreateType::INDEX:
            column_definitions_ = std::nullopt;
            collection_name_ = std::nullopt;   // 这里保持 nullopt，调用 setter 时设置
            break;
        default:
            // 约定 noexcept，不抛出异常
            break;
    }
}

void CreateStmt::set_object_name(const std::string & name)
{
    object_name_ = name;
}

void CreateStmt::set_is_if_not_exists(bool is_if_not_exists)
{
    is_if_not_exists_ = is_if_not_exists;
}

void CreateStmt::add_column_definition(ColumnDefinition && column)
{
    column_definitions_->emplace_back(std::move(column));
}

void CreateStmt::set_collection_name(const std::string & collection_name)
{
    collection_name_ = collection_name;
}

CreateStmt::CreateType CreateStmt::get_create_type() const noexcept
{
    return create_type_;
}

const std::string & CreateStmt::get_object_name() const noexcept
{
    return object_name_;
}

bool CreateStmt::get_is_if_not_exists() const noexcept
{
    return is_if_not_exists_;
}

const std::optional<std::vector<ColumnDefinition>> & CreateStmt::get_column_definitions() const noexcept
{
    return column_definitions_;
}

const std::optional<std::string> & CreateStmt::get_collection_name() const noexcept
{
    return collection_name_;
}

std::string CreateStmt::debug_string() const
{
    std::ostringstream oss;
    oss << "CreateStmt(";

    // 对象类型
    switch (create_type_) {
        case CreateType::DATABASE:
            oss << "DATABASE";
            break;
        case CreateType::COLLECTION:
            oss << "COLLECTION";
            break;
        case CreateType::INDEX:
            oss << "INDEX";
            break;
        default:
            oss << "UNKNOWN";
            break;
    }

    oss << ", name=" << (object_name_.empty() ? "<none>" : object_name_);

    // 是否跳过存在性检查
    if (is_if_not_exists_) {
        oss << ", if_not_exists=true";
    }

    // 列定义
    if (column_definitions_ && !column_definitions_->empty()) {
        oss << ", columns=[";
        for (std::size_t i = 0; i < column_definitions_->size(); ++i) {
            if (i > 0) {
                oss << ", ";
            }
            const auto & col = column_definitions_->at(i);
            oss << col.get_name() << " ";

            // 类型
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

            // 长度
            if (col.get_length() > 0) {
                oss << "(" << col.get_length() << ")";
            }

            // 属性
            if (col.get_is_primary()) {
                oss << " PRIMARY_KEY";
            }
            if (col.get_is_auto_increment()) {
                oss << " AUTO_INCREMENT";
            }
            if (!col.get_is_nullable()) {
                oss << " NOT_NULL";
            }
        }
        oss << "]";
    }

    // 集合名称
    if (collection_name_ && !collection_name_->empty()) {
        oss << ", collection_name=" << *collection_name_;
    }

    oss << ")";

    return oss.str();
}

} // namespace dreamdb
