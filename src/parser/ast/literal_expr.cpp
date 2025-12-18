#include "dreamdb/parser/ast/literal_expr.h"

#include <sstream>

namespace dreamdb
{

LiteralExpr::LiteralExpr(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::LITERAL_EXPR, line, column)
    , type_(LiteralType::NULL_VALUE)
    , value_(nullptr)
{
}

void LiteralExpr::set_literal_type(LiteralType type) noexcept
{
    type_ = type;
}

LiteralType LiteralExpr::get_literal_type() const noexcept
{
    return type_;
}

void LiteralExpr::set_value(const LiteralValue & value) noexcept
{
    value_ = value;
}

const LiteralValue & LiteralExpr::get_value() const noexcept
{
    return value_;
}

void LiteralExpr::set_null(bool is_null) noexcept
{
    is_null_ = is_null;
}

bool LiteralExpr::is_null() const noexcept
{
    return is_null_;
}

std::string LiteralExpr::debug_string() const
{
    std::ostringstream oss;
    oss << "LiteralExpr(";
    switch (type_) {
        case LiteralType::INTERGER:
            oss << "INTERGER, value=" << std::get<int64_t>(value_);
            break;
        case LiteralType::FLOAT:
            oss << "FLOAT, value=" << std::get<double>(value_);
            break;
        case LiteralType::STRING:
            oss << "STRING, value=" << std::get<std::string>(value_);
            break;
        case LiteralType::BOOLEAN:
            oss << "BOOLEAN, value=" << std::get<bool>(value_);
            break;
        case LiteralType::NULL_VALUE:
            oss << "NULL_VALUE";
            break;
        default:
            oss << "UNKNOWN";
            break;
    }
    oss << ")";

    return oss.str();
}

} // namespace dreamdb
