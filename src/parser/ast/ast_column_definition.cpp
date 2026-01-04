#include "dreamdb/parser/ast/ast_column_definition.h"

namespace dreamdb
{

AstColumnDefinition::AstColumnDefinition()
    : name_(std::nullopt)
    , type_name_(std::nullopt)
    , length_(std::nullopt)
    , precision_(std::nullopt)
    , options_(std::nullopt)
    , is_nullable_(true)         // 默认允许 NULL
    , is_primary_(false)         // 默认不是主键
    , is_auto_increment_(false)  // 默认不是自增
    , default_value_(nullptr)
    , comment_(std::nullopt)
{
}

void AstColumnDefinition::set_name(const std::string & name)
{
    name_ = name;
}

void AstColumnDefinition::set_type_name(const std::string & type_name)
{
    type_name_ = type_name;
}

void AstColumnDefinition::set_length(int length) noexcept
{
    length_ = length;
}

void AstColumnDefinition::set_precision(int precision) noexcept
{
    precision_ = precision;
}

void AstColumnDefinition::set_options(const std::vector<std::string> & options)
{
    options_ = options;
}

void AstColumnDefinition::set_is_nullable(bool is_nullable) noexcept
{
    is_nullable_ = is_nullable;
}

void AstColumnDefinition::set_is_primary(bool is_primary) noexcept
{
    is_primary_ = is_primary;
}

void AstColumnDefinition::set_is_auto_increment(bool is_auto_increment) noexcept
{
    is_auto_increment_ = is_auto_increment;
}

void AstColumnDefinition::set_default_value(std::unique_ptr<AstExpressionNode> default_value) noexcept
{
    default_value_ = std::move(default_value);
}

void AstColumnDefinition::set_comment(const std::string & comment)
{
    comment_ = comment;
}

const std::string & AstColumnDefinition::get_name() const
{
    return name_.value();
}

const std::string & AstColumnDefinition::get_type_name() const
{
    return type_name_.value();
}

int AstColumnDefinition::get_length() const
{
    return length_.value();
}

int AstColumnDefinition::get_precision() const
{
    return precision_.value();
}

const std::vector<std::string> & AstColumnDefinition::get_options() const
{
    return options_.value();
}

bool AstColumnDefinition::get_is_nullable() const noexcept
{
    return is_nullable_;
}

bool AstColumnDefinition::get_is_primary() const noexcept
{
    return is_primary_;
}

bool AstColumnDefinition::get_is_auto_increment() const noexcept
{
    return is_auto_increment_;
}

const AstExpressionNode & AstColumnDefinition::get_default_value() const
{
    return *default_value_;
}

const std::string & AstColumnDefinition::get_comment() const
{
    return comment_.value();
}

bool AstColumnDefinition::has_name() const noexcept
{
    return name_.has_value();
}

bool AstColumnDefinition::has_type_name() const noexcept
{
    return type_name_.has_value();
}

bool AstColumnDefinition::has_length() const noexcept
{
    return length_.has_value();
}

bool AstColumnDefinition::has_precision() const noexcept
{
    return precision_.has_value();
}

bool AstColumnDefinition::has_options() const noexcept
{
    return options_.has_value();
}

bool AstColumnDefinition::has_default_value() const noexcept
{
    return default_value_ != nullptr;
}

bool AstColumnDefinition::has_comment() const noexcept
{
    return comment_.has_value();
}

} // namespace dreamdb
