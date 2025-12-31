#pragma once

#include <cstdint>

namespace dreamdb
{

/**
 * @brief 表达式类型
 */
enum class ExpressionType : std::uint8_t
{
    EXPRESSION_COLUMN_REFERENCE,              // 字段引用
    EXPRESSION_CONSTANT,                      // 常量
    EXPRESSION_BINARY,                        // 二元运算符
    EXPRESSION_UNARY,                         // 一元运算符
    EXPRESSION_FUNCTION,                      // 函数
    EXPRESSION_INLIST,                        // IN 列表
    EXPRESSION_LIKE,                          // LIKE 运算符
    EXPRESSION_BETWEEN,                       // BETWEEN 运算符
    EXPRESSION_NULL_CHECK                     // NULL 检查
};

/**
 * @brief 表达式
 */
class Expression
{
public:
    explicit Expression(ExpressionType type) noexcept;

    virtual ~Expression() noexcept = default;

public:
    ExpressionType get_type() const noexcept;

private:
    ExpressionType type_;              // 表达式类型
};

} // namespace dreamdb
