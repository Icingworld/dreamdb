#include "dreamdb/parser/ast/column_definition.h"

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

void ColumnDefinition::set_options(std::vector<std::string> && options)
{
    options_ = std::move(options);
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

} // namespace dreamdb
