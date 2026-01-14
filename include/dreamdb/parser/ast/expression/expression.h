#pragma once

#include <cstdint>
#include <cstddef>

#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb::parser::ast
{

class AstExpressionVisitor;

/**
 * @brief Ast 表达式类型
 */
enum class AstExpressionType : std::uint8_t
{
    Literal,           // 字面量表达式
    ColumnReference,   // 列引用表达式
    Unary,             // 一元表达式
    Binary,            // 二元表达式
    FunctionCall,      // 函数调用表达式
    In,                // IN 表达式
    Between,           // BETWEEN 表达式
    Like,              // LIKE 表达式
    Vector,            // 向量表达式
};

/**
 * @brief 表达式节点基类
 * @details 每个具体的表达式节点都继承自此类
 */
class AstExpression : public AstNode
{
protected:
    AstExpression(AstExpressionType type, std::size_t line, std::size_t column) noexcept;

public:
    ~AstExpression() noexcept override = default;

public:
    /**
     * @brief 获取表达式类型
     * @return 表达式类型
     */
    AstExpressionType type() const noexcept;

    /**
     * @brief 接受表达式访问者
     * @param visitor 表达式访问者
     */
    virtual void accept(AstExpressionVisitor & visitor) const = 0;

private:
    AstExpressionType type_;    // 表达式类型
};

} // namespace dreamdb::parser::ast
