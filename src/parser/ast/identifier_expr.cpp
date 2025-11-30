#include "dreamdb/parser/ast/identifier_expr.h"

#include <sstream>

namespace dreamdb
{

IdentifierExpr::IdentifierExpr(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::IDENTIFIER_EXPR, line, column)
    , type(IdentifierType::COLLECTION)
    , parts()
    , original_text()
{
}

void IdentifierExpr::set_type(IdentifierType type) noexcept
{
    this->type = type;
}

IdentifierType IdentifierExpr::get_type() const noexcept
{
    return this->type;
}

void IdentifierExpr::set_parts(const std::vector<std::string> & parts) noexcept
{
    this->parts = parts;
}

void IdentifierExpr::add_part(const std::string & part) noexcept
{
    this->parts.push_back(part);
}

const std::vector<std::string> & IdentifierExpr::get_parts() const noexcept
{
    return this->parts;
}

void IdentifierExpr::set_original_text(const std::string & original_text) noexcept
{
    this->original_text = original_text;
}

const std::string & IdentifierExpr::get_original_text() const noexcept
{
    return this->original_text;
}

std::string IdentifierExpr::debug_string() const
{
    std::ostringstream oss;
    oss << "IdentifierExpr(type=";
    switch (type) {
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
    if (parts.empty()) {
        oss << "<none>";
    } else {
        for (std::size_t i = 0; i < parts.size(); ++i) {
            if (i > 0) {
                oss << '.';
            }
            oss << parts[i];
        }
    }

    if (!original_text.empty()) {
        oss << ", original='" << original_text << "'";
    }

    oss << ")";
    return oss.str();
}

} // namespace dreamdb
