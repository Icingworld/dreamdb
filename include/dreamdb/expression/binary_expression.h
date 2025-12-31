#pragma once

#include <cstdint>
#include <memory>

#include "dreamdb/expression/expression.h"

namespace dreamdb
{

enum class BinaryOperatorType : std::uint8_t
{
    /** 算数运算符 */
    EXPRESSION_PLUS,            // +
    EXPRESSION_MINUS,           // -
    EXPRESSION_MULTIPLY,        // *
    EXPRESSION_DIVIDE,          // /
    EXPRESSION_MODULO,          // %

    /** 比较运算符 */
    EXPRESSION_EQUAL,           // =
    EXPRESSION_NOT_EQUAL,       // !=
    EXPRESSION_GREATER_THAN,    // >
    EXPRESSION_GREATER_EQUAL,   // >=
    EXPRESSION_LESS_THAN,       // <
    EXPRESSION_LESS_EQUAL,      // <=

    /** 逻辑运算符 */
    EXPRESSION_AND,             // AND
    EXPRESSION_OR               // OR
};

/**
 * @brief 二元运算符表达式
 */
class BinaryExpression : public Expression
{
public:
    explicit BinaryExpression(BinaryOperatorType operator_type, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right);

    BinaryExpression(const BinaryExpression &) noexcept = delete;

    BinaryExpression(BinaryExpression &&) noexcept = default;

    BinaryExpression &operator=(const BinaryExpression &) noexcept = delete;

    BinaryExpression &operator=(BinaryExpression &&) noexcept = default;

    ~BinaryExpression() noexcept = default;

public:
    /**
     * @brief 获取二元运算符类型
     * @return 二元运算符类型
     */
    BinaryOperatorType get_operator_type() const noexcept;

    /**
     * @brief 获取左操作数
     * @return 左操作数表达式
     */
    const Expression & get_left() const noexcept;

    /**
     * @brief 获取右操作数
     * @return 右操作数表达式
     */
    const Expression & get_right() const noexcept;

    /**
     * @brief 获取可变左操作数
     * @return 可变左操作数表达式
     * @details 该接口供 Optimizer 使用
     */
    Expression & get_mutable_left() noexcept;

    /**
     * @brief 获取可变右操作数
     * @return 可变右操作数表达式
     * @details 该接口供 Optimizer 使用
     */
    Expression & get_mutable_right() noexcept;

private:
    BinaryOperatorType operator_type_;            // 二元运算符类型
    std::unique_ptr<Expression> left_;            // 左操作数表达式
    std::unique_ptr<Expression> right_;           // 右操作数表达式
};

} // namespace dreamdb
