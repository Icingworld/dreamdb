#pragma once

#include <cstdint>
#include <cstddef>
#include <memory>

#include "dreamdb/parser/ast/ast_expression_node.h"

namespace dreamdb
{

/**
 * @brief 一元运算符类型
 */
enum class AstUnaryOperatorType : std::uint8_t
{
    AST_UNARY_OPERATOR_UNKNOWN,    // 未知运算符
    AST_UNARY_OPERATOR_NOT,        // 逻辑非
    AST_UNARY_OPERATOR_MINUS,      // 负号
    AST_UNARY_OPERATOR_PLUS        // 正号
};

/**
 * @brief 一元表达式节点
 */
class AstUnaryExpressionNode : public AstExpressionNode
{
public:
    AstUnaryExpressionNode(std::size_t line = 0, std::size_t column = 0);

    AstUnaryExpressionNode(const AstUnaryExpressionNode &) = delete;

    AstUnaryExpressionNode(AstUnaryExpressionNode &&) noexcept = default;

    AstUnaryExpressionNode & operator=(const AstUnaryExpressionNode &) = delete;

    AstUnaryExpressionNode & operator=(AstUnaryExpressionNode &&) noexcept = default;

    ~AstUnaryExpressionNode() override = default;

public:
    /**
     * @brief 设置一元运算符类型
     * @param operator_type 运算符类型
     */
    void set_operator_type(AstUnaryOperatorType operator_type) noexcept;

    /**
     * @brief 设置操作数
     * @param operand 操作数表达式
     */
    void set_operand(std::unique_ptr<AstExpressionNode> operand) noexcept;

    /**
     * @brief 获取一元运算符类型
     * @return 运算符类型
     */
    AstUnaryOperatorType get_operator_type() const noexcept;

    /**
     * @brief 获取操作数
     * @return 操作数表达式
     */
    const AstExpressionNode & get_operand() const;

    /**
     * @brief 是否存在一元运算符类型
     * @return 是否存在一元运算符类型
     */
    bool has_operator_type() const noexcept;

    /**
     * @brief 是否存在操作数
     * @return 是否存在操作数
     */
    bool has_operand() const noexcept;

private:
    AstUnaryOperatorType operator_type_;              // 一元运算符类型
    std::unique_ptr<AstExpressionNode> operand_;      // 操作数表达式
};

} // namespace dreamdb
