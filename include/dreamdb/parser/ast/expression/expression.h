#pragma once

#include <cstdint>
#include <cstddef>

#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb::parser::ast
{

/**
 * @brief Ast 表达式类型
 */
enum class AstExpressionType : std::uint8_t
{
    AST_EXPRESSION_LITERAL,           // 字面量表达式
    AST_EXPRESSION_COLUMN_REFERENCE,  // 列引用表达式
    AST_EXPRESSION_UNARY,             // 一元表达式
    AST_EXPRESSION_BINARY,            // 二元表达式
    AST_EXPRESSION_FUNCTION_CALL,     // 函数调用表达式
    AST_EXPRESSION_PARAMETER,         // 参数表达式
    AST_EXPRESSION_IN,                // IN 表达式
    AST_EXPRESSION_BETWEEN,           // BETWEEN 表达式
    AST_EXPRESSION_LIKE,              // LIKE 表达式
    AST_EXPRESSION_VECTOR,            // 向量表达式
    // 后续如需要，在这里可扩展：CAST | CASE WHEN | SUBQUERY 等表达式
};

/**
 * @brief 表达式节点基类
 * @details 每个具体的表达式节点都继承自此类
 */
class AstExpression : public AstNode
{
protected:
    AstExpression(AstExpressionType expression_type, std::size_t line = 0, std::size_t column = 0) noexcept;

public:
    AstExpression(const AstExpression &) = delete;

    AstExpression(AstExpression &&) noexcept = default;

    AstExpression & operator=(const AstExpression &) = delete;

    AstExpression & operator=(AstExpression &&) noexcept = default;

    ~AstExpression() noexcept override = default;

public:
    /**
     * @brief 获取表达式节点类型
     * @return 表达式节点类型
     */
    AstExpressionType type() const noexcept;

private:
    AstExpressionType type_;    // Ast 表达式类型
};

} // namespace dreamdb::parser::ast
