#include "dreamdb/parser/ast/ast_in_expression_node.h"

namespace dreamdb
{

AstInExpressionNode::AstInExpressionNode(std::size_t line, std::size_t column)
    : AstExpressionNode(AstExpressionNodeType::AST_EXPRESSION_IN, line, column)
    , left_(nullptr)
    , is_not_(false)
{
}

void AstInExpressionNode::set_left(std::unique_ptr<AstExpressionNode> left) noexcept
{
    left_ = std::move(left);
}

void AstInExpressionNode::add_value(std::unique_ptr<AstExpressionNode> value) noexcept
{
    values_.push_back(std::move(value));
}

void AstInExpressionNode::set_is_not(bool is_not) noexcept
{
    is_not_ = is_not;
}

const AstExpressionNode & AstInExpressionNode::get_left() const
{
    return *left_;
}

const std::vector<std::unique_ptr<AstExpressionNode>> & AstInExpressionNode::get_values() const
{
    return values_;
}

bool AstInExpressionNode::is_not() const noexcept
{
    return is_not_;
}

bool AstInExpressionNode::has_left() const noexcept
{
    return left_ != nullptr;
}

bool AstInExpressionNode::has_values() const noexcept
{
    return !values_.empty();
}

} // namespace dreamdb
