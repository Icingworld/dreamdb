#pragma once

#include <cstdint>
#include <cstddef>
#include <memory>

#include "dreamdb/parser/ast/expression/expression.h"

namespace dreamdb::parser::ast
{

/**
 * @brief 一元运算符类型
 */
enum class AstUnaryOperatorType : std::uint8_t
{
    Not,        // 逻辑非
    Minus,      // 算数负号
    Plus,       // 算数正号
};

/**
 * @brief 一元表达式节点
 */
class AstUnaryExpression : public AstExpression
{
public:
    AstUnaryExpression(AstUnaryOperatorType type, std::unique_ptr<AstExpression> operand, std::size_t line, std::size_t column);

    ~AstUnaryExpression() noexcept override = default;

public:
    /**
     * @brief 创建一元表达式
     * @param type 一元运算符类型
     * @param operand 操作数表达式
     * @param line 行号
     * @param column 列号
     * @return 一元表达式
     */
    static std::unique_ptr<AstUnaryExpression> create(
        AstUnaryOperatorType type,
        std::unique_ptr<AstExpression> operand,
        std::size_t line,
        std::size_t column
    );

public:
    /**
     * @brief 获取一元运算符类型
     * @return 运算符类型
     */
    AstUnaryOperatorType type() const noexcept;

    /**
     * @brief 获取操作数表达式
     * @return 操作数表达式
     */
    const AstExpression & operand() const noexcept;

    /**
     * @brief 接受表达式访问者
     * @param visitor 表达式访问者
     */
    void accept(AstExpressionVisitor & visitor) const override;

private:
    AstUnaryOperatorType type_;                 // 一元运算符类型
    std::unique_ptr<AstExpression> operand_;    // 操作数表达式
};

} // namespace dreamdb::parser::ast
