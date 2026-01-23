#pragma once

#include <string>
#include <sstream>

#include "dreamdb/binder/bound/expression/visitor.h"

namespace dreamdb::binder::bound
{

class BoundExpression;

/**
 * @brief 绑定表达式格式化器
 */
class BoundExpressionFormatter final : public BoundExpressionVisitor
{
public:
    BoundExpressionFormatter() noexcept;

    ~BoundExpressionFormatter() noexcept override = default;

public:
    /**
     * @brief 格式化表达式
     * @param expression 表达式
     * @return 格式化后的表达式
     */
    std::string format(const BoundExpression & expression);

private:
    void visit(const BoundColumnReferenceExpression & column_reference_expression) override;

    void visit(const BoundConstantExpression & constant_expression) override;

    void visit(const BoundFunctionCallExpression & function_call_expression) override;

    void visit(const BoundInExpression & in_expression) override;

    void visit(const BoundBetweenExpression & between_expression) override;

    void visit(const BoundLikeExpression & like_expression) override;

    void visit(const BoundUnaryExpression & unary_expression) override;

    void visit(const BoundBinaryExpression & binary_expression) override;

private:
    std::ostringstream oss_;     // 格式化后的表达式字符串流
};

} // namespace dreamdb::binder::bound
