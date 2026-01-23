#pragma once

namespace dreamdb::binder::bound
{

class BoundColumnReferenceExpression;
class BoundConstantExpression;
class BoundFunctionCallExpression;
class BoundInExpression;
class BoundBetweenExpression;
class BoundLikeExpression;
class BoundUnaryExpression;
class BoundBinaryExpression;

/**
 * @brief 绑定表达式访问者
 */
class BoundExpressionVisitor
{
public:
    virtual ~BoundExpressionVisitor() noexcept = default;

public:

    virtual void visit(const BoundColumnReferenceExpression & column_reference_expression) = 0;

    virtual void visit(const BoundConstantExpression & constant_expression) = 0;

    virtual void visit(const BoundFunctionCallExpression & function_call_expression) = 0;

    virtual void visit(const BoundInExpression & in_expression) = 0;

    virtual void visit(const BoundBetweenExpression & between_expression) = 0;

    virtual void visit(const BoundLikeExpression & like_expression) = 0;

    virtual void visit(const BoundUnaryExpression & unary_expression) = 0;

    virtual void visit(const BoundBinaryExpression & binary_expression) = 0;
};

} // namespace dreamdb::binder::bound
