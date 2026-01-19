#pragma once

#include <cstdint>
#include <cstddef>
#include <memory>

#include "dreamdb/parser/ast/expression/expression.h"

namespace dreamdb::parser::ast
{

/**
 * @brief 二元运算符类型
 */
enum class AstBinaryOperatorType : std::uint8_t
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
    Or,             // OR
};

/**
 * @brief 二元表达式
 */
class AstBinaryExpression final : public AstExpression
{
public:
    AstBinaryExpression(
        AstBinaryOperatorType type,
        std::unique_ptr<AstExpression> left,
        std::unique_ptr<AstExpression> right,
        std::size_t line,
        std::size_t column
    );

    ~AstBinaryExpression() noexcept override = default;

public:
    /**
     * @brief 创建二元表达式
     * @param type 二元运算符类型
     * @param left 左操作数表达式
     * @param right 右操作数表达式
     * @param line 行号
     * @param column 列号
     * @return 二元表达式
     */
    static std::unique_ptr<AstBinaryExpression> create(
        AstBinaryOperatorType type,
        std::unique_ptr<AstExpression> left,
        std::unique_ptr<AstExpression> right,
        std::size_t line,
        std::size_t column
    );

public:
    /**
     * @brief 获取二元运算符类型
     * @return 运算符类型
     */
    AstBinaryOperatorType type() const noexcept;

    /**
     * @brief 获取左操作数
     * @return 左操作数表达式指针
     */
    const AstExpression & left() const noexcept;

    /**
     * @brief 获取右操作数
     * @return 右操作数表达式指针
     */
    const AstExpression & right() const noexcept;

    /**
     * @brief 接受表达式访问者
     * @param visitor 表达式访问者
     */
    void accept(AstExpressionVisitor & visitor) const override;

private:
    AstBinaryOperatorType type_;            // 二元运算符类型
    std::unique_ptr<AstExpression> left_;   // 左操作数表达式
    std::unique_ptr<AstExpression> right_;  // 右操作数表达式
};

} // namespace dreamdb::parser::ast
