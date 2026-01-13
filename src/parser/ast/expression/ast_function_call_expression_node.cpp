#include "dreamdb/parser/ast/ast_function_call_expression_node.h"

namespace dreamdb
{

AstFunctionCallExpressionNode::AstFunctionCallExpressionNode(std::size_t line, std::size_t column)
    : AstExpressionNode(AstExpressionNodeType::AST_EXPRESSION_FUNCTION_CALL, line, column)
    , function_name_()
    , arguments_()
{
}

void AstFunctionCallExpressionNode::set_function_name(const std::string & function_name)
{
    function_name_ = function_name;
}

void AstFunctionCallExpressionNode::add_argument(std::unique_ptr<AstExpressionNode> arg)
{
    arguments_.push_back(std::move(arg));
}

const std::string & AstFunctionCallExpressionNode::get_function_name() const
{
    return function_name_.value();
}

const std::vector<std::unique_ptr<AstExpressionNode>> & AstFunctionCallExpressionNode::get_arguments() const noexcept
{
    return arguments_;
}

bool AstFunctionCallExpressionNode::has_function_name() const noexcept
{
    return function_name_.has_value();
}

bool AstFunctionCallExpressionNode::has_arguments() const noexcept
{
    return !arguments_.empty();
}

} // namespace dreamdb
