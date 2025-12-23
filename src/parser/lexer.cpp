#include "dreamdb/parser/lexer.h"

#include <cctype>
#include <stdexcept>

namespace dreamdb
{

Lexer::Lexer(const std::string & input)
    : input_(input)
    , position_(0)
    , line_(1)
    , column_(1)
    , peeked_token_(TokenType::DB_EOF_TOKEN, "", 0, 0)
    , has_peeked_(false)
{
}

std::size_t Lexer::get_position() const noexcept
{
    return position_;
}

std::size_t Lexer::get_line() const noexcept
{
    return line_;
}

std::size_t Lexer::get_column() const noexcept
{
    return column_;
}

Token Lexer::next_token()
{
    // 如果已经预读，直接返回预读的 Token
    if (has_peeked_) {
        has_peeked_ = false;
        return peeked_token_;
    }

    // 跳过空白字符
    skip_whitespace();

    // 检查是否到达末尾
    if (position_ >= input_.length()) {
        return Token(TokenType::DB_EOF_TOKEN, "", line_, column_);
    }

    char current = peek();
    std::size_t start_line = line_;
    std::size_t start_column = column_;

    // 识别标识符和关键字（以字母或下划线开头）
    if (is_alpha(current) || current == '_') {
        return read_identifier_or_keyword();
    }

    // 识别数字（以数字开头）
    if (is_digit(current)) {
        return read_number();
    }

    // 识别字符串字面量（以单引号或双引号开头）
    if (current == '\'' || current == '"') {
        return read_string();
    }

    // 识别运算符和分隔符
    switch (current) {
        case '=':
            advance();
            return Token(TokenType::DB_EQUAL, "=", start_line, start_column);
        case '!':
            advance();
            if (match('=')) {
                return Token(TokenType::DB_NOT_EQUAL, "!=", start_line, start_column);
            }
            return Token(TokenType::DB_NOT, "!", start_line, start_column);
        case '<':
            advance();
            if (match('=')) {
                return Token(TokenType::DB_LESS_EQUAL, "<=", start_line, start_column);
            }
            return Token(TokenType::DB_LESS_THAN, "<", start_line, start_column);
        case '>':
            advance();
            if (match('=')) {
                return Token(TokenType::DB_GREATER_EQUAL, ">=", start_line, start_column);
            }
            return Token(TokenType::DB_GREATER_THAN, ">", start_line, start_column);
        case '+':
            advance();
            return Token(TokenType::DB_PLUS, "+", start_line, start_column);
        case '-':
            advance();
            return Token(TokenType::DB_MINUS, "-", start_line, start_column);
        case '*':
            advance();
            return Token(TokenType::DB_MULTIPLY, "*", start_line, start_column);
        case '/':
            advance();
            return Token(TokenType::DB_DIVIDE, "/", start_line, start_column);
        case '%':
            advance();
            return Token(TokenType::DB_MODULO, "%", start_line, start_column);
        case ',':
            advance();
            return Token(TokenType::DB_COMMA, ",", start_line, start_column);
        case ';':
            advance();
            return Token(TokenType::DB_SEMICOLON, ";", start_line, start_column);
        case '.':
            advance();
            return Token(TokenType::DB_DOT, ".", start_line, start_column);
        case '(':
            advance();
            return Token(TokenType::DB_LEFT_PAREN, "(", start_line, start_column);
        case ')':
            advance();
            return Token(TokenType::DB_RIGHT_PAREN, ")", start_line, start_column);
        case '[':
            advance();
            return Token(TokenType::DB_LEFT_BRACKET, "[", start_line, start_column);
        case ']':
            advance();
            return Token(TokenType::DB_RIGHT_BRACKET, "]", start_line, start_column);
        case '{':
            advance();
            return Token(TokenType::DB_LEFT_BRACE, "{", start_line, start_column);
        case '}':
            advance();
            return Token(TokenType::DB_RIGHT_BRACE, "}", start_line, start_column);
        default:
            // 未知字符
            char unknown = advance();
            throw std::runtime_error(
                "Unexpected character '" + std::string(1, unknown) + 
                "' at line " + std::to_string(start_line) + 
                ", column " + std::to_string(start_column)
            );
    }
}

Token Lexer::peek_token()
{
    if (!has_peeked_) {
        peeked_token_ = next_token();
        has_peeked_ = true;
    }

    return peeked_token_;
}

bool Lexer::has_more() const noexcept
{
    return position_ < input_.length();
}

void Lexer::skip_whitespace()
{
    while (position_ < input_.length()) {
        char c = peek();
        if (c == ' ' || c == '\t') {
            advance();
            ++column_;
        } else if (c == '\n') {
            advance();
            ++line_;
            column_ = 1;
        } else if (c == '\r') {
            advance();
            // Windows 换行符 \r\n，跳过 \r
            if (peek() == '\n') {
                advance();
            }
            ++line_;
            column_ = 1;
        } else {
            break;
        }
    }
}

Token Lexer::read_identifier_or_keyword()
{
    std::size_t start = position_;
    std::size_t start_line = line_;
    std::size_t start_column = column_;

    // 读取连续的字母、数字、下划线
    while (position_ < input_.length() && is_alnum(peek())) {
        advance();
        ++column_;
    }

    std::string text = input_.substr(start, position_ - start);

    // 检查是否为关键字
    TokenType type = keyword_to_token_type(text);
    if (type != TokenType::DB_IDENTIFIER) {
        return Token(type, "", start_line, start_column);
    }

    // 是标识符
    return Token(TokenType::DB_IDENTIFIER, text, start_line, start_column);
}

Token Lexer::read_number()
{
    std::size_t start = position_;
    std::size_t start_line = line_;
    std::size_t start_column = column_;

    // 读取整数部分
    while (position_ < input_.length() && is_digit(peek())) {
        advance();
        ++column_;
    }

    // 检查是否有小数点（浮点数）
    if (peek() == '.' && position_ + 1 < input_.length() && is_digit(input_[position_ + 1])) {
        advance(); // 跳过小数点
        ++column_;
        
        // 读取小数部分
        while (position_ < input_.length() && is_digit(peek())) {
            advance();
            ++column_;
        }
    }

    std::string text = input_.substr(start, position_ - start);
    return Token(TokenType::DB_NUMBER_LITERAL, text, start_line, start_column);
}

Token Lexer::read_string()
{
    std::size_t start_line = line_;
    std::size_t start_column = column_;
    char quote = advance(); // 跳过开始引号
    ++column_;

    std::string value;
    bool escaped = false;

    while (position_ < input_.length()) {
        char c = peek();
        
        if (escaped) {
            // 处理转义字符
            switch (c) {
                case 'n': value += '\n';
                    break;
                case 't': value += '\t';
                    break;
                case 'r': value += '\r';
                    break;
                case '\\': value += '\\';
                    break;
                case '\'': value += '\'';
                    break;
                case '"': value += '"';
                    break;
                default: value += c;
                    break;
            }
            escaped = false;
            advance();
            ++column_;
        } else if (c == '\\') {
            escaped = true;
            advance();
            ++column_;
        } else if (c == quote) {
            advance(); // 跳过结束引号
            ++column_;
            return Token(TokenType::DB_STRING_LITERAL, value, start_line, start_column);
        } else if (c == '\n') {
            throw std::runtime_error(
                "Unterminated string literal at line " + std::to_string(start_line) + 
                ", column " + std::to_string(start_column)
            );
        } else {
            value += c;
            advance();
            ++column_;
        }
    }

    throw std::runtime_error(
        "Unterminated string literal at line " + std::to_string(start_line) + 
        ", column " + std::to_string(start_column)
    );
}

bool Lexer::is_alpha(char c) const noexcept
{
    return std::isalpha(static_cast<unsigned char>(c)) != 0;
}

bool Lexer::is_digit(char c) const noexcept
{
    return std::isdigit(static_cast<unsigned char>(c)) != 0;
}

bool Lexer::is_alnum(char c) const noexcept
{
    return std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_';
}

char Lexer::peek() const
{
    if (position_ >= input_.length()) {
        return '\0';
    }
    return input_[position_];
}

char Lexer::advance()
{
    if (position_ >= input_.length()) {
        return '\0';
    }
    return input_[position_++];
}

bool Lexer::match(char expected)
{
    if (peek() == expected) {
        advance();
        ++column_;
        return true;
    }
    return false;
}

TokenType Lexer::keyword_to_token_type(const std::string & keyword) const
{
    // 将关键字字符串转换为 TokenType
    std::string upper_keyword = keyword;
    for (char & c : upper_keyword) {
        c = std::toupper(static_cast<unsigned char>(c));
    }

    if (upper_keyword == "SELECT") {
        return TokenType::DB_SELECT;
    }
    if (upper_keyword == "INSERT") {
        return TokenType::DB_INSERT;
    }
    if (upper_keyword == "DELETE") {
        return TokenType::DB_DELETE;
    }
    if (upper_keyword == "UPDATE") {
        return TokenType::DB_UPDATE;
    }
    if (upper_keyword == "CREATE") {
        return TokenType::DB_CREATE;
    }
    if (upper_keyword == "DROP") {
        return TokenType::DB_DROP;
    }
    if (upper_keyword == "USE") {
        return TokenType::DB_USE;
    }
    if (upper_keyword == "ALTER") {
        return TokenType::DB_ALTER;
    }
    if (upper_keyword == "SHOW") {
        return TokenType::DB_SHOW;
    }
    if (upper_keyword == "DESCRIBE") {
        return TokenType::DB_DESCRIBE;
    }
    if (upper_keyword == "FROM") {
        return TokenType::DB_FROM;
    }
    if (upper_keyword == "WHERE") {
        return TokenType::DB_WHERE;
    }
    if (upper_keyword == "INTO") {
        return TokenType::DB_INTO;
    }
    if (upper_keyword == "VALUES") {
        return TokenType::DB_VALUES;
    }
    if (upper_keyword == "SET") {
        return TokenType::DB_SET;
    }
    if (upper_keyword == "AND") {
        return TokenType::DB_AND;
    }
    if (upper_keyword == "OR") {
        return TokenType::DB_OR;
    }
    if (upper_keyword == "NOT") {
        return TokenType::DB_NOT;
    }
    if (upper_keyword == "AS") {
        return TokenType::DB_AS;
    }
    if (upper_keyword == "ON") {
        return TokenType::DB_ON;
    }
    if (upper_keyword == "OFFSET") {
        return TokenType::DB_OFFSET;
    }
    if (upper_keyword == "DATABASE") {
        return TokenType::DB_DATABASE;
    }
    if (upper_keyword == "COLLECTION") {
        return TokenType::DB_COLLECTION;
    }
    if (upper_keyword == "INDEX") {
        return TokenType::DB_INDEX;
    }
    if (upper_keyword == "LIMIT") {
        return TokenType::DB_LIMIT;
    }
    if (upper_keyword == "PRIMARY") {
        return TokenType::DB_PRIMARY;
    }
    if (upper_keyword == "KEY") {
        return TokenType::DB_KEY;
    }
    if (upper_keyword == "AUTO_INCREMENT") {
        return TokenType::DB_AUTO_INCREMENT;
    }
    if (upper_keyword == "DEFAULT") {
        return TokenType::DB_DEFAULT;
    }
    if (upper_keyword == "UNIQUE") {
        return TokenType::DB_UNIQUE;
    }
    if (upper_keyword == "IF") {
        return TokenType::DB_IF;
    }
    if (upper_keyword == "EXISTS") {
        return TokenType::DB_EXISTS;
    }
    if (upper_keyword == "ORDER") {
        return TokenType::DB_ORDER;
    }
    if (upper_keyword == "BY") {
        return TokenType::DB_BY;
    }
    if (upper_keyword == "ASC") {
        return TokenType::DB_ASC;
    }
    if (upper_keyword == "DESC") {
        return TokenType::DB_DESC;
    }
    if (upper_keyword == "BETWEEN") {
        return TokenType::DB_BETWEEN;
    }
    if (upper_keyword == "IN") {
        return TokenType::DB_IN;
    }
    if (upper_keyword == "LIKE") {
        return TokenType::DB_LIKE;
    }
    if (upper_keyword == "IS") {
        return TokenType::DB_IS;
    }
    if (upper_keyword == "SEARCH") {
        return TokenType::DB_SEARCH;
    }
    if (upper_keyword == "USING") {
        return TokenType::DB_USING;
    }
    if (upper_keyword == "WITH") {
        return TokenType::DB_WITH;
    }
    if (upper_keyword == "TRUE") {
        return TokenType::DB_TRUE;
    }
    if (upper_keyword == "FALSE") {
        return TokenType::DB_FALSE;
    }
    if (upper_keyword == "NULL") {
        return TokenType::DB_NULL;
    }

    return TokenType::DB_IDENTIFIER;
}

} // namespace dreamdb
