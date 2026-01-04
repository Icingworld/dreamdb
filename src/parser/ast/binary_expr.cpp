#include "dreamdb/parser/ast/binary_expr.h"

namespace dreamdb
{

AstBinaryExpressionNode::AstBinaryExpressionNode(std::size_t line, std::size_t column)
    : AstExpressionNode(AstExpressionNodeType::AST_EXPRESSION_BINARY, line, column)
    , operator_type_(AstBinaryOperatorType::AST_BINARY_OPERATOR_UNKNOWN)
    , left_(nullptr)
    , right_(nullptr)
{
}

void AstBinaryExpressionNode::set_operator_type(AstBinaryOperatorType operator_type) noexcept
{
    operator_type_ = operator_type;
}

void AstBinaryExpressionNode::set_left(std::unique_ptr<AstExpressionNode> left) noexcept
{
    left_ = std::move(left);
}

void AstBinaryExpressionNode::set_right(std::unique_ptr<AstExpressionNode> right) noexcept
{
    right_ = std::move(right);
}

AstBinaryOperatorType AstBinaryExpressionNode::get_operator_type() const noexcept
{
    return operator_type_;
}

const AstExpressionNode & AstBinaryExpressionNode::get_left() const
{
    return *left_;
}

const AstExpressionNode & AstBinaryExpressionNode::get_right() const
{
    return *right_;
}

bool AstBinaryExpressionNode::has_operator_type() const noexcept
{
    return operator_type_ != AstBinaryOperatorType::AST_BINARY_OPERATOR_UNKNOWN;
}

bool AstBinaryExpressionNode::has_left() const noexcept
{
    return left_ != nullptr;
}

bool AstBinaryExpressionNode::has_right() const noexcept
{
    return right_ != nullptr;
}

} // namespace dreamdb
