#pragma once

#include <cstddef>

#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb::parser::ast
{

class AstExpressionVisitor;

/**
 * @brief 表达式节点基类
 * @details 每个具体的表达式节点都继承自此类
 */
class AstExpression : public AstNode
{
protected:
    AstExpression(std::size_t line, std::size_t column) noexcept;

public:
    ~AstExpression() noexcept override = default;

public:
    /**
     * @brief 接受表达式访问者
     * @param visitor 表达式访问者
     */
    virtual void accept(AstExpressionVisitor & visitor) const = 0;
};

} // namespace dreamdb::parser::ast
