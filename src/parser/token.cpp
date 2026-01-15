#include "dreamdb/parser/token.h"

namespace dreamdb::parser
{

Token::Token(
    TokenType type,
    std::string value,
    std::size_t line,
    std::size_t column
)
    : type_(type)
    , value_(value)
    , line_(line)
    , column_(column)
{
}

TokenType Token::type() const noexcept
{
    return type_;
}

const std::string & Token::value() const noexcept
{
    return value_;
}

std::size_t Token::line() const noexcept
{
    return line_;
}

std::size_t Token::column() const noexcept
{
    return column_;
}

} // namespace dreamdb::parser
