#pragma once

#include <cstdint>
#include <memory>

#include "dreamdb/expression/expression.h"

namespace dreamdb
{

/**
 * @brief 一元运算符类型
 */
enum class UnaryOperatorType : std::uint8_t
{
    EXPRESSION_NOT,       // NOT
    EXPRESSION_MINUS,     // -
    EXPRESSION_PLUS       // +
};

/**
 * @brief 一元运算符表达式
 */
class UnaryExpression : public Expression
{
public:
    explicit UnaryExpression(UnaryOperatorType operator_type, std::unique_ptr<Expression> operand);

    UnaryExpression(const UnaryExpression &) noexcept = delete;

    UnaryExpression(UnaryExpression &&) noexcept = default;

    UnaryExpression &operator=(const UnaryExpression &) noexcept = delete;

    UnaryExpression &operator=(UnaryExpression &&) noexcept = default;

    ~UnaryExpression() noexcept = default;

public:
    /**
     * @brief 获取一元运算符类型
     * @return 一元运算符类型
     */
    UnaryOperatorType get_operator_type() const noexcept;

    /**
     * @brief 获取操作数
     * @return 操作数表达式
     */
    const Expression & get_operand() const noexcept;

    /**
     * @brief 获取可变操作数
     * @return 可变操作数表达式
     * @details 该接口供 Optimizer 使用
     */
    Expression & get_mutable_operand() noexcept;

private:
    UnaryOperatorType operator_type_;            // 一元运算符类型
    std::unique_ptr<Expression> operand_;        // 操作数表达式
};

} // namespace dreamdb
