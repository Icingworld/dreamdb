#pragma once

#include <cstdint>
#include <cstddef>

#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb
{

/**
 * @brief 表达式节点类型
 */
enum class AstExpressionNodeType : std::uint8_t
{
    AST_EXPRESSION_LITERAL,           // 字面量表达式
    AST_EXPRESSION_IDENTIFIER,        // 标识符表达式
    AST_EXPRESSION_UNARY,             // 一元表达式
    AST_EXPRESSION_BINARY,            // 二元表达式
    AST_EXPRESSION_FUNCTION_CALL,     // 函数调用表达式
    AST_EXPRESSION_IN,                // IN 表达式
    AST_EXPRESSION_LIKE,              // LIKE 表达式
    AST_EXPRESSION_BETWEEN,           // BETWEEN 表达式
    AST_EXPRESSION_NULL_CHECK,        // NULL 检查表达式
};

/**
 * @brief 表达式节点基类
 * @details 每个具体的表达式节点都继承自此类
 */
class AstExpressionNode : public AstNode
{
protected:
    AstExpressionNode(AstExpressionNodeType expression_type, std::size_t line = 0, std::size_t column = 0) noexcept;

public:
    AstExpressionNode(const AstExpressionNode &) = delete;

    AstExpressionNode(AstExpressionNode &&) noexcept = default;

    AstExpressionNode & operator=(const AstExpressionNode &) = delete;

    AstExpressionNode & operator=(AstExpressionNode &&) noexcept = default;

    ~AstExpressionNode() noexcept override = default;

public:
    /**
     * @brief 获取表达式节点类型
     * @return 表达式节点类型
     */
    AstExpressionNodeType get_expression_type() const noexcept;

private:
    AstExpressionNodeType expression_type_;    // 表达式节点类型
};

} // namespace dreamdb
