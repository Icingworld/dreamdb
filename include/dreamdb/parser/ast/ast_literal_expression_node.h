#pragma once

#include <cstdint>
#include <cstddef>
#include <memory>
#include <variant>
#include <string>
#include <vector>

#include "dreamdb/parser/ast/ast_expression_node.h"

namespace dreamdb
{

/**
 * @brief 字面量类型
 */
enum class AstLiteralType : std::uint8_t
{
    AST_LITERAL_INTEGER,       // 整数
    AST_LITERAL_FLOAT,         // 浮点数
    AST_LITERAL_STRING,        // 字符串
    AST_LITERAL_BOOLEAN,       // 布尔值
    AST_LITERAL_NULL           // 空值
};

/**
 * @brief 字面量表达式
 */
class AstLiteralExpressionNode : public AstExpressionNode
{
private:
    AstLiteralExpressionNode(std::size_t line = 0, std::size_t column = 0);

public:
    AstLiteralExpressionNode(const AstLiteralExpressionNode &) = delete;

    AstLiteralExpressionNode(AstLiteralExpressionNode &&) noexcept = default;

    AstLiteralExpressionNode & operator=(const AstLiteralExpressionNode &) = delete;

    AstLiteralExpressionNode & operator=(AstLiteralExpressionNode &&) noexcept = default;

    ~AstLiteralExpressionNode() override = default;

public:
    /**
     * @brief 创建整数字面量表达式
     * @param value 整数值
     * @return 整数字面量表达式
     */
    static std::unique_ptr<AstLiteralExpressionNode> create_integer(std::int64_t value);

    /**
     * @brief 创建浮点字面量表达式
     * @param value 浮点数值
     * @return 浮点字面量表达式
     */
    static std::unique_ptr<AstLiteralExpressionNode> create_float(double value);

    /**
     * @brief 创建字符串字面量表达式
     * @param value 字符串值
     * @return 字符串字面量表达式
     */
    static std::unique_ptr<AstLiteralExpressionNode> create_string(const std::string & value);

    /**
     * @brief 创建布尔字面量表达式
     * @param value 布尔值
     * @return 布尔字面量表达式
     */
    static std::unique_ptr<AstLiteralExpressionNode> create_boolean(bool value);

    /**
     * @brief 创建空字面量表达式
     * @return 空字面量表达式
     */
    static std::unique_ptr<AstLiteralExpressionNode> create_null();

public:
    /**
     * @brief 获取字面量类型
     * @return 字面量类型
     */
    AstLiteralType get_literal_type() const noexcept;

    /**
     * @brief 获取整数值
     * @return 整数值
     */
    std::int64_t get_integer() const;

    /**
     * @brief 获取浮点数值
     * @return 浮点数值
     */
    double get_float() const;

    /**
     * @brief 获取字符串值
     * @return 字符串值
     */
    const std::string & get_string() const;

    /**
     * @brief 获取布尔值
     * @return 布尔值
     */
    bool get_boolean() const;

    /**
     * @brief 是否为整数
     * @return 是否为整数
     */
    bool is_integer() const noexcept;

    /**
     * @brief 是否为浮点数
     * @return 是否为浮点数
     */
    bool is_float() const noexcept;

    /**
     * @brief 是否为字符串
     * @return 是否为字符串
     */
    bool is_string() const noexcept;

    /**
     * @brief 是否为布尔值
     * @return 是否为布尔值
     */
    bool is_boolean() const noexcept;

    /**
     * @brief 是否为空值
     * @return 是否为空值
     */
    bool is_null() const noexcept;

private:
    AstLiteralType literal_type_;      // 字面量类型

    /** 字面量值 */
    bool bool_value_;                  // 布尔值
    std::int64_t int_value_;           // 整数值
    double float_value_;               // 浮点数值
    std::string string_value_;         // 字符串值
};

} // namespace dreamdb
