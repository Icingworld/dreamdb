#include "dreamdb/parser/ast/ast_like_expression_node.h"

namespace dreamdb
{

AstLikeExpressionNode::AstLikeExpressionNode(std::size_t line, std::size_t column)
    : AstExpressionNode(AstExpressionNodeType::AST_EXPRESSION_LIKE, line, column)
    , left_(nullptr)
    , pattern_(nullptr)
    , is_not_(false)
{
}

void AstLikeExpressionNode::set_left(std::unique_ptr<AstExpressionNode> left) noexcept
{
    left_ = std::move(left);
}

void AstLikeExpressionNode::set_pattern(std::unique_ptr<AstExpressionNode> pattern) noexcept
{
    pattern_ = std::move(pattern);
}

void AstLikeExpressionNode::set_is_not(bool is_not) noexcept
{
    is_not_ = is_not;
}

const AstExpressionNode & AstLikeExpressionNode::get_left() const
{
    return *left_;
}

const AstExpressionNode & AstLikeExpressionNode::get_pattern() const
{
    return *pattern_;
}

bool AstLikeExpressionNode::is_not() const noexcept
{
    return is_not_;
}

bool AstLikeExpressionNode::has_left() const noexcept
{
    return left_ != nullptr;
}

bool AstLikeExpressionNode::has_pattern() const noexcept
{
    return pattern_ != nullptr;
}

} // namespace dreamdb
