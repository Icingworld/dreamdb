#include "dreamdb/parser/ast/statement/column_definition.h"

#include <cassert>

#include "dreamdb/parser/ast/expression/expression.h"

namespace dreamdb::parser::ast
{

AstColumnDefinition::AstColumnDefinition(
    std::string name,
    std::string type_name,
    std::vector<std::unique_ptr<AstExpression>> arguments,
    std::vector<AstColumnModifier> modifiers,
    std::unique_ptr<AstExpression> default_value,
    std::optional<std::string> comment
)
    : name_(std::move(name))
    , type_name_(std::move(type_name))
    , arguments_(std::move(arguments))
    , modifiers_(std::move(modifiers))
    , default_value_(std::move(default_value))
    , comment_(std::move(comment))
{
    // 列名不能为空
    assert(!name_.empty());

    // 字段类型不能为空
    assert(!type_name_.empty());

    // 每个参数表达式都不能为空
    for (const auto & arg : arguments_) {
        assert(arg != nullptr);
    }
}

const std::string & AstColumnDefinition::name() const noexcept
{
    return name_;
}

const std::string & AstColumnDefinition::type_name() const noexcept
{
    return type_name_;
}

const AstExpression & AstColumnDefinition::argument_at(std::size_t index) const noexcept
{
    // 索引不能超出范围
    assert(index < arguments_.size());

    return *arguments_[index];
}

std::size_t AstColumnDefinition::argument_count() const noexcept
{
    return arguments_.size();
}

AstColumnModifier AstColumnDefinition::modifier_at(std::size_t index) const noexcept
{
    // 索引不能超出范围
    assert(index < modifiers_.size());

    return modifiers_[index];
}

std::size_t AstColumnDefinition::modifier_count() const noexcept
{
    return modifiers_.size();
}

bool AstColumnDefinition::has_default_value() const noexcept
{
    return default_value_ != nullptr;
}

const AstExpression * AstColumnDefinition::default_value() const noexcept
{
    return default_value_.get();
}

const AstExpression & AstColumnDefinition::default_value_ref() const noexcept
{
    // 默认值表达式必须存在
    assert(default_value_ != nullptr);

    return *default_value_;
}

bool AstColumnDefinition::has_comment() const noexcept
{
    return comment_.has_value();
}

const std::string & AstColumnDefinition::comment() const noexcept
{
    // 注释必须存在
    assert(comment_.has_value());

    return comment_.value();
}

} // namespace dreamdb::parser::ast
