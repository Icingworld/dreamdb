#include "dreamdb/parser/ast/ast_parameter_expression_node.h"

namespace dreamdb
{

AstParameterExpressionNode::AstParameterExpressionNode(std::size_t line, std::size_t column)
    : AstExpressionNode(AstExpressionNodeType::AST_EXPRESSION_PARAMETER, line, column)
    , parameter_index_(std::nullopt)
{
}

void AstParameterExpressionNode::set_parameter_index(std::size_t parameter_index)
{
    parameter_index_ = parameter_index;
}

std::size_t AstParameterExpressionNode::get_parameter_index() const noexcept
{
    return parameter_index_.value();
}

bool AstParameterExpressionNode::has_parameter_index() const noexcept
{
    return parameter_index_.has_value();
}

} // namespace dreamdb