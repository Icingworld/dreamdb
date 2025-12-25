#include "dreamdb/parser/ast/like_expr.h"

#include <sstream>

namespace dreamdb
{

LikeExpr::LikeExpr(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::LIKE_EXPR, line, column)
    , left_(nullptr)
    , is_not_like_(false)
    , right_(nullptr)
{
}

void LikeExpr::set_left(std::unique_ptr<AstNode> left) noexcept
{
    left_ = std::move(left);
}

void LikeExpr::set_is_not_like(bool is_not_like) noexcept
{
    is_not_like_ = is_not_like;
}

void LikeExpr::set_right(std::unique_ptr<AstNode> right) noexcept
{
    right_ = std::move(right);
}

const AstNode * LikeExpr::get_left() const noexcept
{
    return left_.get();
}

bool LikeExpr::is_not_like() const noexcept
{
    return is_not_like_;
}

const AstNode * LikeExpr::get_right() const noexcept
{
    return right_.get();
}

std::string LikeExpr::debug_string() const
{
    std::ostringstream oss;
    oss << "LikeExpr(left=";

    if (left_) {
        oss << left_->debug_string();
    } else {
        oss << "<null>";
    }

    if (is_not_like_) {
        oss << ", NOT LIKE";
    } else {
        oss << ", LIKE";
    }

    oss << ", right=";
    if (right_) {
        oss << right_->debug_string();
    } else {
        oss << "<null>";
    }

    oss << ")";

    return oss.str();
}

} // namespace dreamdb
