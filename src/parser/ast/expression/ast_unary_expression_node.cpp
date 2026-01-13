#include "dreamdb/parser/ast/ast_unary_expression_node.h"

namespace dreamdb
{

AstUnaryExpressionNode::AstUnaryExpressionNode(std::size_t line, std::size_t column)
    : AstExpressionNode(AstExpressionNodeType::AST_EXPRESSION_UNARY, line, column)
    , operator_type_(AstUnaryOperatorType::AST_UNARY_OPERATOR_UNKNOWN)
    , operand_(nullptr)
{
}

void AstUnaryExpressionNode::set_operator_type(AstUnaryOperatorType operator_type) noexcept
{
    operator_type_ = operator_type;
}

void AstUnaryExpressionNode::set_operand(std::unique_ptr<AstExpressionNode> operand) noexcept
{
    operand_ = std::move(operand);
}

AstUnaryOperatorType AstUnaryExpressionNode::get_operator_type() const noexcept
{
    return operator_type_;
}

const AstExpressionNode & AstUnaryExpressionNode::get_operand() const
{
    return *operand_;
}

bool AstUnaryExpressionNode::has_operator_type() const noexcept
{
    return operator_type_ != AstUnaryOperatorType::AST_UNARY_OPERATOR_UNKNOWN;
}

bool AstUnaryExpressionNode::has_operand() const noexcept
{
    return operand_ != nullptr;
}

} // namespace dreamdb
