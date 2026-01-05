#include "dreamdb/parser/ast/ast_column_definition.h"

namespace dreamdb
{

AstColumnDefinition::AstColumnDefinition()
    : name_(std::nullopt)
    , type_name_(std::nullopt)
    , arguments_()
    , modifiers_()
    , default_values_()
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

void AstColumnDefinition::add_argument(std::unique_ptr<AstExpressionNode> argument) noexcept
{
    arguments_.push_back(std::move(argument));
}

void AstColumnDefinition::add_modifier(AstColumnModifier modifier) noexcept
{
    modifiers_.push_back(modifier);
}

void AstColumnDefinition::add_default_value(std::unique_ptr<AstExpressionNode> default_value) noexcept
{
    default_values_.push_back(std::move(default_value));
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

const std::vector<std::unique_ptr<AstExpressionNode>> & AstColumnDefinition::get_arguments() const
{
    return arguments_;
}

const std::vector<AstColumnModifier> & AstColumnDefinition::get_modifiers() const
{
    return modifiers_;
}

const std::vector<std::unique_ptr<AstExpressionNode>> & AstColumnDefinition::get_default_values() const
{
    return default_values_;
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

bool AstColumnDefinition::has_arguments() const noexcept
{
    return !arguments_.empty();
}

bool AstColumnDefinition::has_modifiers() const noexcept
{
    return !modifiers_.empty();
}

bool AstColumnDefinition::has_default_values() const noexcept
{
    return !default_values_.empty();
}

bool AstColumnDefinition::has_comment() const noexcept
{
    return comment_.has_value();
}

} // namespace dreamdb
