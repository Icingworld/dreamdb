#pragma once

#include <cstdint>
#include <memory>

#include "dreamdb/binder/bound/expression/expression.h"
#include "dreamdb/common/logical_type.h"

namespace dreamdb::binder::bound
{

class BoundExpressionVisitor;

/**
 * @brief 绑定后的二元运算符类型
 */
enum class BoundBinaryOperatorType : std::uint8_t
{
    // 算术运算符
    Plus,           // +
    Minus,          // -
    Multiply,       // *
    Divide,         // /
    Modulo,         // %

    // 比较运算符
    Equal,          // =
    NotEqual,       // != 或 <>
    LessThan,       // <
    GreaterThan,    // >
    LessEqual,      // <=
    GreaterEqual,   // >=

    // 逻辑运算符
    And,            // AND
    Or              // OR
};

/**
 * @brief 绑定后的二元表达式
 */
class BoundBinaryExpression final : public BoundExpression
{
public:
    explicit BoundBinaryExpression(
        BoundBinaryOperatorType operator_type,
        std::unique_ptr<BoundExpression> left,
        std::unique_ptr<BoundExpression> right,
        dreamdb::common::LogicalType logical_type
    );

    ~BoundBinaryExpression() noexcept override = default;

public:
    /**
     * @brief 获取二元运算符类型
     * @return 二元运算符类型
     */
    BoundBinaryOperatorType operator_type() const noexcept;

    /**
     * @brief 获取左操作数
     * @return 左操作数表达式
     */
    const BoundExpression & left() const noexcept;

    /**
     * @brief 获取右操作数
     * @return 右操作数表达式
     */
    const BoundExpression & right() const noexcept;

    /**
     * @brief 接受表达式访问者
     * @param visitor 表达式访问者
     */
    void accept(BoundExpressionVisitor & visitor) const override;

private:
    BoundBinaryOperatorType operator_type_;            // 二元运算符类型
    std::unique_ptr<BoundExpression> left_;            // 左操作数表达式
    std::unique_ptr<BoundExpression> right_;           // 右操作数表达式
};

} // namespace dreamdb::binder::bound
