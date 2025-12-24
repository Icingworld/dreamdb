#include "dreamdb/parser/ast/null_expr.h"

#include <sstream>

namespace dreamdb
{

NullExpr::NullExpr(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::NULL_EXPR, line, column)
    , left_(nullptr)
    , is_not_null_(false)
{
}

void NullExpr::set_left(std::unique_ptr<AstNode> left) noexcept
{
    left_ = std::move(left);
}

void NullExpr::set_is_not_null(bool is_not_null) noexcept
{
    is_not_null_ = is_not_null;
}

const AstNode * NullExpr::get_left() const noexcept
{
    return left_.get();
}

bool NullExpr::is_not_null() const noexcept
{
    return is_not_null_;
}

std::string NullExpr::debug_string() const
{
    std::ostringstream oss;
    oss << "NullExpr(left=";

    if (left_) {
        oss << left_->debug_string();
    } else {
        oss << "<null>";
    }

    if (is_not_null_) {
        oss << ", IS NOT NULL";
    } else {
        oss << ", IS NULL";
    }

    oss << ")";

    return oss.str();
}

} // namespace dreamdb
