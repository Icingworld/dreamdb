#pragma once

#include <string>
#include <vector>
#include <optional>
#include <memory>

#include "dreamdb/parser/ast/ast_node.h"
#include "dreamdb/common/type.h"

namespace dreamdb
{

/**
 * @brief 列定义
 */
class ColumnDefinition
{
public:
    ColumnDefinition();

    ColumnDefinition(const ColumnDefinition &) = delete;

    ColumnDefinition(ColumnDefinition &&) noexcept = default;

    ColumnDefinition & operator=(const ColumnDefinition &) = delete;

    ColumnDefinition & operator=(ColumnDefinition &&) noexcept = default;

    ~ColumnDefinition() = default;

public:
    /**
    * @brief 设置列名
    * @param name 列名
    */
    void set_name(const std::string & name);

    /**
    * @brief 设置字段类型
    * @param type 字段类型
    */
    void set_type(FieldType type);

    /**
    * @brief 设置长度
    * @param length 长度
    */
    void set_length(int length);

    /**
    * @brief 设置精度
    * @param precision 精度
    */
    void set_precision(int precision);

    /**
    * @brief 设置字段选项
    * @param options ENUM 字段选项
    */
    void set_options(std::vector<std::string> && options);

    /**
    * @brief 设置是否允许 NULL
    */
    void set_is_nullable(bool is_nullable);

    /**
    * @brief 设置是否为主键
    */
    void set_is_primary(bool is_primary);

    /**
    * @brief 设置字段注释
    * @param comment 字段注释
    */
    void set_comment(const std::string & comment);

    /**
    * @brief 设置默认值表达式（AST 节点）
    */
    void set_default_value(std::unique_ptr<AstNode> expr);

    /**
    * @brief 设置字段是否自动递增
    * @param is_auto_increment 字段是否自动递增
    */
    void set_is_auto_increment(bool is_auto_increment);

    /**
    * @brief 获取列名
    * @return 列名
    */
    const std::string & get_name() const noexcept;

    /**
    * @brief 获取字段类型
    * @return 字段类型
    */
    FieldType get_type() const noexcept;

    /**
    * @brief 获取长度
    * @return 长度
    */
    int get_length() const noexcept;

    /**
    * @brief 获取精度
    * @return 精度
    */
    int get_precision() const noexcept;

    /**
    * @brief 获取字段选项
    * @return 字段选项
    */
    const std::optional<std::vector<std::string>> & get_options() const noexcept;

    /**
    * @brief 是否允许 NULL
    * @return 是否允许 NULL
    */
    bool get_is_nullable() const noexcept;

    /**
    * @brief 是否为主键
    * @return 是否为主键
    */
    bool get_is_primary() const noexcept;

    /**
    * @brief 获取字段注释
    * @return 字段注释
    */
    const std::string & get_comment() const noexcept;

    /**
    * @brief 获取默认值表达式
    * @return 默认值表达式
    */
    const AstNode * get_default_value() const noexcept;

    /**
    * @brief 是否自动递增
    * @return 是否自动递增
    */
    bool get_is_auto_increment() const noexcept;

private:
    std::string name_;                                 // 列名
    FieldType type_;                                   // 字段类型
    int length_;                                       // 长度
    int precision_;                                    // 精度
    std::optional<std::vector<std::string>> options_;  // ENUM 字段选项
    bool is_nullable_;                                 // 是否允许 NULL
    bool is_primary_;                                  // 是否为主键
    std::string comment_;                              // 字段注释
    std::unique_ptr<AstNode> default_value_;           // 默认值表达式
    bool is_auto_increment_;                           // 是否自动递增
};
 

} // namespace dreamdb
