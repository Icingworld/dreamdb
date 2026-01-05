#include "dreamdb/parser/token.h"

namespace dreamdb
{

Token::Token(TokenType type, std::optional<std::string> value, std::size_t line, std::size_t column)
    : type_(type)
    , value_(std::move(value))
    , line_(line)
    , column_(column)
{
}

TokenType Token::get_type() const noexcept
{
    return type_;
}

const std::string & Token::get_value() const
{
    return value_.value();
}

std::size_t Token::get_line() const noexcept
{
    return line_;
}

std::size_t Token::get_column() const noexcept
{
    return column_;
}

bool Token::has_value() const noexcept
{
    return value_.has_value();
}

} // namespace dreamdb
