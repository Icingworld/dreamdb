#include "dreamdb/parser/ast/literal_expr.h"

#include <sstream>

namespace dreamdb
{

LiteralExpr::LiteralExpr(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::LITERAL_EXPR, line, column)
    , type(LiteralType::NULL_VALUE)
    , value(nullptr)
{
}

void LiteralExpr::set_literal_type(LiteralType type) noexcept
{
    this->type = type;
}

LiteralType LiteralExpr::get_literal_type() const noexcept
{
    return type;
}

void LiteralExpr::set_value(const LiteralValue & value) noexcept
{
    this->value = value;
}

const LiteralValue & LiteralExpr::get_value() const noexcept
{
    return value;
}

void LiteralExpr::set_null(bool is_null) noexcept
{
    value = NullType();
}

bool LiteralExpr::is_null() const noexcept
{
    return value.index() == 4; // 该实现有待考量
}

std::string LiteralExpr::debug_string() const
{
    std::ostringstream oss;
    oss << "LiteralExpr(";
    switch (type) {
        case LiteralType::INTERGER:
            oss << "INTERGER, value=" << std::get<int64_t>(value);
            break;
        case LiteralType::FLOAT:
            oss << "FLOAT, value=" << std::get<double>(value);
            break;
        case LiteralType::STRING:
            oss << "STRING, value=" << std::get<std::string>(value);
            break;
        case LiteralType::BOOLEAN:
            oss << "BOOLEAN, value=" << std::get<bool>(value);
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
