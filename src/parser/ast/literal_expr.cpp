#include "dreamdb/parser/ast/literal_expr.h"

#include <sstream>

namespace dreamdb
{

LiteralExpr::LiteralExpr(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::LITERAL_EXPR, line, column)
    , literal_type_(LiteralType::NULL_VALUE)
    , literal_value_(Null{})
{
}

void LiteralExpr::set_literal_type(LiteralType literal_type) noexcept
{
    literal_type_ = literal_type;
}

void LiteralExpr::set_literal_value(const LiteralValue & literal_value)
{
    literal_value_ = literal_value;
}

LiteralExpr::LiteralType LiteralExpr::get_literal_type() const noexcept
{
    return literal_type_;
}

const LiteralExpr::LiteralValue & LiteralExpr::get_literal_value() const noexcept
{
    return literal_value_;
}

std::string LiteralExpr::debug_string() const
{
    std::ostringstream oss;
    oss << "LiteralExpr(";
    switch (literal_type_) {
        case LiteralType::INTEGER:
            oss << "INTEGER, value=" << std::get<int64_t>(literal_value_);
            break;
        case LiteralType::FLOAT:
            oss << "FLOAT, value=" << std::get<double>(literal_value_);
            break;
        case LiteralType::STRING:
            oss << "STRING, value=\"" << std::get<std::string>(literal_value_) << "\"";
            break;
        case LiteralType::BOOLEAN:
            oss << "BOOLEAN, value=" << (std::get<bool>(literal_value_) ? "true" : "false");
            break;
        case LiteralType::NULL_VALUE:
            oss << "NULL_VALUE";
            break;
        case LiteralType::VECTOR: {
            oss << "VECTOR, value=[";
            const auto & vec = std::get<std::vector<float>>(literal_value_);
            for (std::size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) {
                    oss << ", ";
                }
                oss << vec[i];
            }
            oss << "]";
            break;
        }
        default:
            oss << "UNKNOWN";
            break;
    }
    oss << ")";

    return oss.str();
}

} // namespace dreamdb
