#pragma once

namespace dreamdb::parser::ast
{

class AstLiteralExpression;
class AstColumnReferenceExpression;
class AstUnaryExpression;
class AstBinaryExpression;
class AstFunctionCallExpression;
class AstInExpression;
class AstBetweenExpression;
class AstLikeExpression;
class AstVectorExpression;

/**
 * @brief 表达式访问者基类
 * @details 用于访问表达式节点的各个子节点
 */
class AstExpressionVisitor
{
public:
    virtual ~AstExpressionVisitor() noexcept = default;

public:
    virtual void visit(const AstLiteralExpression & expression) = 0;

    virtual void visit(const AstColumnReferenceExpression & expression) = 0;

    virtual void visit(const AstUnaryExpression & expression) = 0;

    virtual void visit(const AstBinaryExpression & expression) = 0;

    virtual void visit(const AstFunctionCallExpression & expression) = 0;

    virtual void visit(const AstInExpression & expression) = 0;

    virtual void visit(const AstBetweenExpression & expression) = 0;

    virtual void visit(const AstLikeExpression & expression) = 0;

    virtual void visit(const AstVectorExpression & expression) = 0;
};

} // namespace dreamdb::parser::ast
