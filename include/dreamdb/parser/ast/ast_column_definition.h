#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <optional>
#include <memory>

#include "dreamdb/parser/ast/ast_expression_node.h"

namespace dreamdb
{

/**
 * @brief 列修饰符
 */
enum class AstColumnModifier : std::uint8_t
{
    AST_COLUMN_MODIFIER_NOT_NULL,            // NOT NULL          
    AST_COLUMN_MODIFIER_UNIQUE,              // UNIQUE
    AST_COLUMN_MODIFIER_PRIMARY_KEY,         // PRIMARY KEY
    AST_COLUMN_MODIFIER_AUTO_INCREMENT,      // AUTO_INCREMENT
    AST_COLUMN_MODIFIER_DEFAULT,             // DEFAULT
};

/**
 * @brief 列定义
 */
class AstColumnDefinition
{
public:
    AstColumnDefinition();

    AstColumnDefinition(const AstColumnDefinition &) = delete;

    AstColumnDefinition(AstColumnDefinition &&) noexcept = default;

    AstColumnDefinition & operator=(const AstColumnDefinition &) = delete;

    AstColumnDefinition & operator=(AstColumnDefinition &&) noexcept = default;

    ~AstColumnDefinition() = default;

public:
    /**
     * @brief 设置列名
     * @param name 列名
     */
    void set_name(const std::string & name);

    /**
     * @brief 设置字段类型
     * @param type_name 字段类型
     */
    void set_type_name(const std::string & type_name);

    /**
     * @brief 添加参数
     * @param argument 参数
     */
    void add_argument(std::unique_ptr<AstExpressionNode> argument) noexcept;

    /**
     * @brief 添加列修饰符
     * @param modifier 列修饰符
     */
    void add_modifier(AstColumnModifier modifier) noexcept;

    /**
     * @brief 添加默认值表达式
     * @param default_value 默认值表达式
     */
    void add_default_value(std::unique_ptr<AstExpressionNode> default_value) noexcept;

    /**
     * @brief 设置字段注释
     * @param comment 字段注释
     */
    void set_comment(const std::string & comment);

    /**
     * @brief 获取列名
     * @return 列名
     */
    const std::string & get_name() const;

    /**
     * @brief 获取字段类型
     * @return 字段类型
     */
    const std::string & get_type_name() const;

    /**
     * @brief 获取参数列表
     * @return 参数列表
     */
    const std::vector<std::unique_ptr<AstExpressionNode>> & get_arguments() const;

    /**
     * @brief 获取列修饰符列表
     * @return 列修饰符列表
     */
    const std::vector<AstColumnModifier> & get_modifiers() const;

    /**
     * @brief 获取默认值表达式列表
     * @return 默认值表达式列表
     */
    const std::vector<std::unique_ptr<AstExpressionNode>> & get_default_values() const;

    /**
     * @brief 获取字段注释
     * @return 字段注释
     */
    const std::string & get_comment() const;

    /**
     * @brief 是否存在列名
     * @return 是否存在列名
     */
    bool has_name() const noexcept;

    /**
     * @brief 是否存在字段类型
     * @return 是否存在字段类型
     */
    bool has_type_name() const noexcept;

    /**
     * @brief 是否存在参数列表
     * @return 是否存在参数列表
     */
    bool has_arguments() const noexcept;

    /**
     * @brief 是否存在列修饰符
     * @return 是否存在列修饰符
     */
    bool has_modifiers() const noexcept;

    /**
     * @brief 是否存在默认值表达式列表
     * @return 是否存在默认值表达式列表
     */
    bool has_default_values() const noexcept;

    /**
     * @brief 是否存在字段注释
     * @return 是否存在字段注释
     */
    bool has_comment() const noexcept;

private:
    std::optional<std::string> name_;                  // 列名
    std::optional<std::string> type_name_;             // 字段类型
    std::vector<std::unique_ptr<AstExpressionNode>> arguments_;         // 参数列表
    std::vector<AstColumnModifier> modifiers_;         // 列修饰符列表
    std::vector<std::unique_ptr<AstExpressionNode>> default_values_;    // 默认值表达式列表
    std::optional<std::string> comment_;               // 字段注释
};

} // namespace dreamdb
