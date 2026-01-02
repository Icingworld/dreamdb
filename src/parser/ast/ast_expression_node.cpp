#include "dreamdb/parser/ast/ast_expression_node.h"

namespace dreamdb
{

AstExpressionNode::AstExpressionNode(AstExpressionNodeType expression_type, std::size_t line, std::size_t column) noexcept
    : AstNode(line, column)
    , expression_type_(expression_type)
{
}

AstExpressionNodeType AstExpressionNode::get_expression_type() const noexcept
{
    return expression_type_;
}

} // namespace dreamdb
