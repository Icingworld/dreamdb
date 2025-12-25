#include "dreamdb/parser/ast/between_expr.h"

#include <sstream>

namespace dreamdb
{

BetweenExpr::BetweenExpr(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::BETWEEN_EXPR, line, column)
    , left_(nullptr)
    , is_not_between_(false)
    , min_value_(nullptr)
    , max_value_(nullptr)
{
}

void BetweenExpr::set_left(std::unique_ptr<AstNode> left) noexcept
{
    left_ = std::move(left);
}

void BetweenExpr::set_is_not_between(bool is_not_between) noexcept
{
    is_not_between_ = is_not_between;
}

void BetweenExpr::set_min_value(std::unique_ptr<AstNode> min_value) noexcept
{
    min_value_ = std::move(min_value);
}

void BetweenExpr::set_max_value(std::unique_ptr<AstNode> max_value) noexcept
{
    max_value_ = std::move(max_value);
}

const AstNode * BetweenExpr::get_left() const noexcept
{
    return left_.get();
}

bool BetweenExpr::is_not_between() const noexcept
{
    return is_not_between_;
}

const AstNode * BetweenExpr::get_min_value() const noexcept
{
    return min_value_.get();
}

const AstNode * BetweenExpr::get_max_value() const noexcept
{
    return max_value_.get();
}

std::string BetweenExpr::debug_string() const
{
    std::ostringstream oss;
    oss << "BetweenExpr(left=";

    if (left_) {
        oss << left_->debug_string();
    } else {
        oss << "<null>";
    }

    if (is_not_between_) {
        oss << ", NOT BETWEEN";
    } else {
        oss << ", BETWEEN";
    }

    oss << ", min=";
    if (min_value_) {
        oss << min_value_->debug_string();
    } else {
        oss << "<null>";
    }

    oss << ", max=";
    if (max_value_) {
        oss << max_value_->debug_string();
    } else {
        oss << "<null>";
    }

    oss << ")";

    return oss.str();
}

} // namespace dreamdb
