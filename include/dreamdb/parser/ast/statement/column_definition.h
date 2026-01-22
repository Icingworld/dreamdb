#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <optional>
#include <memory>

#include "dreamdb/parser/ast/expression/expression.h"

namespace dreamdb::parser::ast
{

class AstExpression;

/**
 * @brief 列修饰符
 */
enum class AstColumnModifier : std::uint8_t
{
    NotNull,            // NOT NULL
    Unique,             // UNIQUE
    PrimaryKey,         // PRIMARY KEY
    AutoIncrement,      // AUTO_INCREMENT
    Default             // DEFAULT
};

/**
 * @brief 列定义
 */
class AstColumnDefinition final
{
public:
    AstColumnDefinition(
        std::string name,
        std::string type_name,
        std::vector<std::unique_ptr<AstExpression>> arguments,
        std::vector<AstColumnModifier> modifiers,
        std::unique_ptr<AstExpression> default_value,
        std::optional<std::string> comment
    );

    AstColumnDefinition(const AstColumnDefinition &) = delete;

    AstColumnDefinition(AstColumnDefinition &&) noexcept = default;

    AstColumnDefinition & operator=(const AstColumnDefinition &) = delete;

    AstColumnDefinition & operator=(AstColumnDefinition &&) noexcept = default;

    ~AstColumnDefinition() noexcept = default;

public:
    /**
     * @brief 获取列名
     * @return 列名
     */
    const std::string & name() const noexcept;

    /**
     * @brief 获取字段类型
     * @return 字段类型
     */
    const std::string & type_name() const noexcept;

    /**
     * @brief 获取指定索引的参数表达式
     * @param index 索引
     * @return 参数表达式
     */
    const AstExpression & argument_at(std::size_t index) const noexcept;

    /**
     * @brief 获取参数数量
     * @return 参数数量
     */
    std::size_t argument_count() const noexcept;

    /**
     * @brief 获取指定索引的列修饰符
     * @param index 索引
     * @return 列修饰符
     */
    AstColumnModifier modifier_at(std::size_t index) const noexcept;

    /**
     * @brief 获取列修饰符数量
     * @return 列修饰符数量
     */
    std::size_t modifier_count() const noexcept;

    /**
     * @brief 是否存在默认值表达式
     * @return 是否存在默认值表达式
     */
    bool has_default_value() const noexcept;

    /**
     * @brief 获取默认值表达式
     * @return 默认值表达式
     */
    const AstExpression * default_value() const noexcept;

    /**
     * @brief 获取默认值表达式
     * @return 默认值表达式
     */
    const AstExpression & default_value_ref() const noexcept;

    /**
     * @brief 是否存在字段注释
     * @return 是否存在字段注释
     */
    bool has_comment() const noexcept;

    /**
     * @brief 获取字段注释
     * @return 字段注释
     */
    const std::string & comment() const noexcept;

private:
    std::string name_;                                         // 列名
    std::string type_name_;                                    // 字段类型
    std::vector<std::unique_ptr<AstExpression>> arguments_;    // 参数列表
    std::vector<AstColumnModifier> modifiers_;                 // 列修饰符列表
    std::unique_ptr<AstExpression> default_value_;             // 默认值表达式
    std::optional<std::string> comment_;                       // 字段注释
};

} // namespace dreamdb::parser::ast
