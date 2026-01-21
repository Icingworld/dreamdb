#include "dreamdb/binder/bound/debug/expression_formatter.h"

#include "dreamdb/binder/bound/expression/expression.h"

namespace dreamdb::binder::bound
{

BoundExpressionFormatter::BoundExpressionFormatter() noexcept
    : oss_()
{
}

std::string BoundExpressionFormatter::format(const BoundExpression & expression)
{
    oss_.str("");
    oss_.clear();
    expression.accept(*this);
    return oss_.str();
}

void BoundExpressionFormatter::visit(const BoundColumnReferenceExpression & column_reference_expression)
{
    // TODO: 实现 BoundColumnReferenceExpression 格式化
    (void)column_reference_expression;
}

void BoundExpressionFormatter::visit(const BoundConstantExpression & constant_expression)
{
    // TODO: 实现 BoundConstantExpression 格式化
    (void)constant_expression;
}

void BoundExpressionFormatter::visit(const BoundFunctionCallExpression & function_call_expression)
{
    // TODO: 实现 BoundFunctionCallExpression 格式化
    (void)function_call_expression;
}

void BoundExpressionFormatter::visit(const BoundInExpression & in_expression)
{
    // TODO: 实现 BoundInExpression 格式化
    (void)in_expression;
}

void BoundExpressionFormatter::visit(const BoundBetweenExpression & between_expression)
{
    // TODO: 实现 BoundBetweenExpression 格式化
    (void)between_expression;
}

void BoundExpressionFormatter::visit(const BoundLikeExpression & like_expression)
{
    // TODO: 实现 BoundLikeExpression 格式化
    (void)like_expression;
}

void BoundExpressionFormatter::visit(const BoundUnaryExpression & unary_expression)
{
    // TODO: 实现 BoundUnaryExpression 格式化
    (void)unary_expression;
}

void BoundExpressionFormatter::visit(const BoundBinaryExpression & binary_expression)
{
    // TODO: 实现 BoundBinaryExpression 格式化
    (void)binary_expression;
}

} // namespace dreamdb::binder::bound
