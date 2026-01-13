#include "dreamdb/parser/ast/ast_between_expression_node.h"

namespace dreamdb
{

AstBetweenExpressionNode::AstBetweenExpressionNode(std::size_t line, std::size_t column)
    : AstExpressionNode(AstExpressionNodeType::AST_EXPRESSION_BETWEEN, line, column)
    , left_(nullptr)
    , start_(nullptr)
    , end_(nullptr)
    , is_not_(false)
{
}

void AstBetweenExpressionNode::set_left(std::unique_ptr<AstExpressionNode> left) noexcept
{
    left_ = std::move(left);
}

void AstBetweenExpressionNode::set_start(std::unique_ptr<AstExpressionNode> start) noexcept
{
    start_ = std::move(start);
}

void AstBetweenExpressionNode::set_end(std::unique_ptr<AstExpressionNode> end) noexcept
{
    end_ = std::move(end);
}

void AstBetweenExpressionNode::set_is_not(bool is_not) noexcept
{
    is_not_ = is_not;
}

const AstExpressionNode & AstBetweenExpressionNode::get_left() const
{
    return *left_;
}

const AstExpressionNode & AstBetweenExpressionNode::get_start() const
{
    return *start_;
}

const AstExpressionNode & AstBetweenExpressionNode::get_end() const
{
    return *end_;
}

bool AstBetweenExpressionNode::is_not() const noexcept
{
    return is_not_;
}

bool AstBetweenExpressionNode::has_left() const noexcept
{
    return left_ != nullptr;
}

bool AstBetweenExpressionNode::has_start() const noexcept
{
    return start_ != nullptr;
}

bool AstBetweenExpressionNode::has_end() const noexcept
{
    return end_ != nullptr;
}

} // namespace dreamdb
