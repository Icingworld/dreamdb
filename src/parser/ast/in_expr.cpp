#include "dreamdb/parser/ast/in_expr.h"

#include <sstream>

namespace dreamdb
{

InExpr::InExpr(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::IN_EXPR, line, column)
    , left_(nullptr)
    , is_not_in_(false)
    , values_()
{
}

void InExpr::set_left(std::unique_ptr<AstNode> left) noexcept
{
    left_ = std::move(left);
}

void InExpr::set_is_not_in(bool is_not_in) noexcept
{
    is_not_in_ = is_not_in;
}

void InExpr::add_value(std::unique_ptr<AstNode> value) noexcept
{
    values_.push_back(std::move(value));
}

const AstNode * InExpr::get_left() const noexcept
{
    return left_.get();
}

bool InExpr::is_not_in() const noexcept
{
    return is_not_in_;
}

const std::vector<std::unique_ptr<AstNode>> & InExpr::get_values() const noexcept
{
    return values_;
}

std::string InExpr::debug_string() const
{
    std::ostringstream oss;
    oss << "InExpr(left=";

    if (left_) {
        oss << left_->debug_string();
    } else {
        oss << "<null>";
    }
    
    if (is_not_in_) {
        oss << ", NOT IN, values=[";
    } else {
        oss << ", IN, values=[";
    }
    
    for (std::size_t i = 0; i < values_.size(); ++i) {
        if (i > 0) {
            oss << ", ";
        }
        if (values_[i]) {
            oss << values_[i]->debug_string();
        } else {
            oss << "<null>";
        }
    }
    oss << "])";

    return oss.str();
}

} // namespace dreamdb
