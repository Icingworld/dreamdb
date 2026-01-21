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
    virtual void visit(const AstLiteralExpression & literal_expression) = 0;

    virtual void visit(const AstColumnReferenceExpression & column_reference_expression) = 0;

    virtual void visit(const AstUnaryExpression & unary_expression) = 0;

    virtual void visit(const AstBinaryExpression & binary_expression) = 0;

    virtual void visit(const AstFunctionCallExpression & function_call_expression) = 0;

    virtual void visit(const AstInExpression & in_expression) = 0;

    virtual void visit(const AstBetweenExpression & between_expression) = 0;

    virtual void visit(const AstLikeExpression & like_expression) = 0;

    virtual void visit(const AstVectorExpression & vector_expression) = 0;
};

} // namespace dreamdb::parser::ast
