#pragma once

#include <string>
#include <sstream>

#include "dreamdb/parser/ast/expression/expression_visitor.h"

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
    void visit(const AstLiteralExpression & expression) override;

    void visit(const AstColumnReferenceExpression & expression) override;

    void visit(const AstUnaryExpression & expression) override;

    void visit(const AstBinaryExpression & expression) override;

    void visit(const AstFunctionCallExpression & expression) override;

    void visit(const AstInExpression & expression) override;

    void visit(const AstBetweenExpression & expression) override;

    void visit(const AstLikeExpression & expression) override;

    void visit(const AstVectorExpression & expression) override;

private:
    std::ostringstream oss_;     // 格式化后的表达式字符串流
};

} // namespace dreamdb::parser::ast
