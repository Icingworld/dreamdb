#pragma once

#include <string>
#include <sstream>

#include "dreamdb/parser/ast/expression/visitor.h"

namespace dreamdb::parser::ast
{

class AstExpression;

/**
 * @brief 表达式格式化器
 */
class AstExpressionFormatter final : public AstExpressionVisitor
{
public:
    AstExpressionFormatter() noexcept;

    ~AstExpressionFormatter() noexcept override = default;

public:
    /**
     * @brief 格式化表达式
     * @param expression 表达式
     * @return 格式化后的表达式
     */
    std::string format(const AstExpression & expression);

private:
    void visit(const AstLiteralExpression & literal_expression) override;

    void visit(const AstColumnReferenceExpression & column_reference_expression) override;

    void visit(const AstUnaryExpression & unary_expression) override;

    void visit(const AstBinaryExpression & binary_expression) override;

    void visit(const AstFunctionCallExpression & function_call_expression) override;

    void visit(const AstInExpression & in_expression) override;

    void visit(const AstBetweenExpression & between_expression) override;

    void visit(const AstLikeExpression & like_expression) override;

    void visit(const AstVectorExpression & vector_expression) override;

private:
    std::ostringstream oss_;     // 格式化后的表达式字符串流
};

} // namespace dreamdb::parser::ast
