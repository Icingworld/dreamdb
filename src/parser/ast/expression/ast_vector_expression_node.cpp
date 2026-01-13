#include "dreamdb/parser/ast/ast_vector_expression_node.h"

namespace dreamdb
{

AstVectorExpressionNode::AstVectorExpressionNode(std::size_t line, std::size_t column)
    : AstExpressionNode(AstExpressionNodeType::AST_EXPRESSION_VECTOR, line, column)
{
}

void AstVectorExpressionNode::add_element(std::unique_ptr<AstExpressionNode> element) noexcept
{
    elements_.push_back(std::move(element));
}

const std::vector<std::unique_ptr<AstExpressionNode>> & AstVectorExpressionNode::get_elements() const noexcept
{
    return elements_;
}

bool AstVectorExpressionNode::has_elements() const noexcept
{
    return !elements_.empty();
}

std::size_t AstVectorExpressionNode::get_size() const noexcept
{
    return elements_.size();
}

} // namespace dreamdb
