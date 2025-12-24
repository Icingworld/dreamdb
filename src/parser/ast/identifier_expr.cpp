#include "dreamdb/parser/ast/identifier_expr.h"

#include <sstream>

namespace dreamdb
{

IdentifierExpr::IdentifierExpr(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::IDENTIFIER_EXPR, line, column)
    , type_(IdentifierType::COLUMN)
    , parts_()
    , original_identifier_()
{
}

void IdentifierExpr::set_identifier_type(IdentifierType type) noexcept
{
    type_ = type;
}

IdentifierExpr::IdentifierType IdentifierExpr::get_identifier_type() const noexcept
{
    return type_;
}

void IdentifierExpr::set_parts(const std::vector<std::string> & parts)
{
    parts_ = parts;
}

void IdentifierExpr::add_part(const std::string & part)
{
    parts_.push_back(part);
}

const std::vector<std::string> & IdentifierExpr::get_parts() const noexcept
{
    return parts_;
}

void IdentifierExpr::set_original_identifier(const std::string & original_identifier)
{
    original_identifier_ = original_identifier;
}

const std::string & IdentifierExpr::get_original_identifier() const
{
    return original_identifier_;
}

std::string IdentifierExpr::debug_string() const
{
    std::ostringstream oss;
    oss << "IdentifierExpr(type=";
    switch (type_) {
        case IdentifierType::COLLECTION:
            oss << "COLLECTION";
            break;
        case IdentifierType::COLUMN:
            oss << "COLUMN";
            break;
        case IdentifierType::ALIAS:
            oss << "ALIAS";
            break;
        case IdentifierType::FUNCTION:
            oss << "FUNCTION";
            break;
        default:
            oss << "UNKNOWN";
            break;
    }

    oss << ", parts=";
    if (parts_.empty()) {
        oss << "<none>";
    } else {
        for (std::size_t i = 0; i < parts_.size(); ++i) {
            if (i > 0) {
                oss << '.';
            }
            oss << parts_[i];
        }
    }

    if (!original_identifier_.empty()) {
        oss << ", original='" << original_identifier_ << "'";
    }

    oss << ")";
    return oss.str();
}

} // namespace dreamdb
