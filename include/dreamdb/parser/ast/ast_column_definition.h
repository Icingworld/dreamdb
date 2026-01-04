#pragma once

#include <string>
#include <vector>
#include <optional>
#include <memory>

#include "dreamdb/parser/ast/ast_expression_node.h"

namespace dreamdb
{

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
     * @brief 设置长度
     * @param length 长度
     */
    void set_length(int length) noexcept;

    /**
     * @brief 设置精度
     * @param precision 精度
     */
    void set_precision(int precision) noexcept;

    /**
     * @brief 设置 ENUM 字段选项
     * @param options ENUM 字段选项
     */
    void set_options(const std::vector<std::string> & options);

    /**
     * @brief 设置是否允许 NULL
     * @param is_nullable 是否允许 NULL
     */
    void set_is_nullable(bool is_nullable) noexcept;

    /**
     * @brief 设置是否为主键
     * @param is_primary 是否为主键
     */
    void set_is_primary(bool is_primary) noexcept;

    /**
     * @brief 设置是否自动递增
     * @param is_auto_increment 是否自动递增
     */
    void set_is_auto_increment(bool is_auto_increment) noexcept;

    /**
     * @brief 设置默认值表达式
     * @param default_value 默认值表达式
     */
    void set_default_value(std::unique_ptr<AstExpressionNode> default_value) noexcept;

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
     * @brief 获取长度
     * @return 长度
     */
    int get_length() const;

    /**
     * @brief 获取精度
     * @return 精度
     */
    int get_precision() const;

    /**
     * @brief 获取 ENUM 字段选项
     * @return ENUM 字段选项
     */
    const std::vector<std::string> & get_options() const;

    /**
     * @brief 获取是否允许 NULL
     * @return 是否允许 NULL
     */
    bool get_is_nullable() const noexcept;

    /**
     * @brief 获取是否为主键
     * @return 是否为主键
     */
    bool get_is_primary() const noexcept;

    /**
     * @brief 获取是否自动递增
     * @return 是否自动递增
     */
    bool get_is_auto_increment() const noexcept;

    /**
     * @brief 获取默认值表达式
     * @return 默认值表达式
     */
    const AstExpressionNode & get_default_value() const;

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
     * @brief 是否存在长度
     * @return 是否存在长度
     */
    bool has_length() const noexcept;

    /**
     * @brief 是否存在精度
     * @return 是否存在精度
     */
    bool has_precision() const noexcept;

    /**
     * @brief 是否存在 ENUM 字段选项
     * @return 是否存在 ENUM 字段选项
     */
    bool has_options() const noexcept;

    /**
     * @brief 是否存在默认值表达式
     * @return 是否存在默认值表达式
     */
    bool has_default_value() const noexcept;

    /**
     * @brief 是否存在字段注释
     * @return 是否存在字段注释
     */
    bool has_comment() const noexcept;

private:
    std::optional<std::string> name_;                  // 列名
    std::optional<std::string> type_name_;             // 字段类型
    std::optional<int> length_;                        // 长度
    std::optional<int> precision_;                     // 精度
    std::optional<std::vector<std::string>> options_;  // ENUM 字段选项
    bool is_nullable_;                                 // 是否允许 NULL
    bool is_primary_;                                  // 是否为主键
    bool is_auto_increment_;                           // 是否自动递增
    std::unique_ptr<AstExpressionNode> default_value_; // 默认值表达式
    std::optional<std::string> comment_;               // 字段注释
};
 

} // namespace dreamdb
