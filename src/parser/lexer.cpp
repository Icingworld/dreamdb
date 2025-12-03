#include "dreamdb/parser/lexer.h"

#include <cctype>
#include <stdexcept>

namespace dreamdb
{

Lexer::Lexer(const std::string & input)
    : input(input)
    , position(0)
    , line(1)
    , column(1)
    , has_peeked(false)
{
}

std::size_t Lexer::get_position() const noexcept
{
    return position;
}

std::size_t Lexer::get_line() const noexcept
{
    return line;
}

std::size_t Lexer::get_column() const noexcept
{
    return column;
}

Token Lexer::next_token()
{
    // 如果已经预读，直接返回预读的 Token
    if (has_peeked) {
        has_peeked = false;
        return peeked_token;
    }

    // 跳过空白字符
    skip_whitespace();

    // 检查是否到达末尾
    if (position >= input.length()) {
        return Token(TokenType::EOF_TOKEN, "", line, column);
    }

    char current = peek();
    std::size_t start_line = line;
    std::size_t start_column = column;

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
            return Token(TokenType::EQUAL, "=", start_line, start_column);
        case '!':
            advance();
            if (match('=')) {
                return Token(TokenType::NOT_EQUAL, "!=", start_line, start_column);
            }
            return Token(TokenType::NOT, "!", start_line, start_column);
        case '<':
            advance();
            if (match('=')) {
                return Token(TokenType::LESS_EQUAL, "<=", start_line, start_column);
            }
            return Token(TokenType::LESS_THAN, "<", start_line, start_column);
        case '>':
            advance();
            if (match('=')) {
                return Token(TokenType::GREATER_EQUAL, ">=", start_line, start_column);
            }
            return Token(TokenType::GREATER_THAN, ">", start_line, start_column);
        case '+':
            advance();
            return Token(TokenType::PLUS, "+", start_line, start_column);
        case '-':
            advance();
            return Token(TokenType::MINUS, "-", start_line, start_column);
        case '*':
            advance();
            return Token(TokenType::MULTIPLY, "*", start_line, start_column);
        case '/':
            advance();
            return Token(TokenType::DIVIDE, "/", start_line, start_column);
        case '%':
            advance();
            return Token(TokenType::MODULO, "%", start_line, start_column);
        case ',':
            advance();
            return Token(TokenType::COMMA, ",", start_line, start_column);
        case ';':
            advance();
            return Token(TokenType::SEMICOLON, ";", start_line, start_column);
        case '.':
            advance();
            return Token(TokenType::DOT, ".", start_line, start_column);
        case '(':
            advance();
            return Token(TokenType::LEFT_PAREN, "(", start_line, start_column);
        case ')':
            advance();
            return Token(TokenType::RIGHT_PAREN, ")", start_line, start_column);
        case '[':
            advance();
            return Token(TokenType::LEFT_BRACKET, "[", start_line, start_column);
        case ']':
            advance();
            return Token(TokenType::RIGHT_BRACKET, "]", start_line, start_column);
        case '{':
            advance();
            return Token(TokenType::LEFT_BRACE, "{", start_line, start_column);
        case '}':
            advance();
            return Token(TokenType::RIGHT_BRACE, "}", start_line, start_column);
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
    if (!has_peeked) {
        peeked_token = next_token();
        has_peeked = true;
    }

    return peeked_token;
}

bool Lexer::has_more() const noexcept
{
    return position < input.length();
}

void Lexer::skip_whitespace()
{
    while (position < input.length()) {
        char c = peek();
        if (c == ' ' || c == '\t') {
            advance();
            ++column;
        } else if (c == '\n') {
            advance();
            ++line;
            column = 1;
        } else if (c == '\r') {
            advance();
            // Windows 换行符 \r\n，跳过 \r
            if (peek() == '\n') {
                advance();
            }
            ++line;
            column = 1;
        } else {
            break;
        }
    }
}

Token Lexer::read_identifier_or_keyword()
{
    std::size_t start = position;
    std::size_t start_line = line;
    std::size_t start_column = column;

    // 读取连续的字母、数字、下划线
    while (position < input.length() && is_alnum(peek())) {
        advance();
        ++column;
    }

    std::string text = input.substr(start, position - start);

    // 检查是否为关键字
    TokenType type = keyword_to_token_type(text);
    if (type != TokenType::IDENTIFIER) {
        return Token(type, "", start_line, start_column);
    }

    // 是标识符
    return Token(TokenType::IDENTIFIER, text, start_line, start_column);
}

Token Lexer::read_number()
{
    std::size_t start = position;
    std::size_t start_line = line;
    std::size_t start_column = column;

    // 读取整数部分
    while (position < input.length() && is_digit(peek())) {
        advance();
        ++column;
    }

    // 检查是否有小数点（浮点数）
    if (peek() == '.' && position + 1 < input.length() && is_digit(input[position + 1])) {
        advance(); // 跳过小数点
        ++column;
        
        // 读取小数部分
        while (position < input.length() && is_digit(peek())) {
            advance();
            ++column;
        }
    }

    std::string text = input.substr(start, position - start);
    return Token(TokenType::NUMBER_LITERAL, text, start_line, start_column);
}

Token Lexer::read_string()
{
    std::size_t start_line = line;
    std::size_t start_column = column;
    char quote = advance(); // 跳过开始引号
    column++;

    std::string value;
    bool escaped = false;

    while (position < input.length()) {
        char c = peek();
        
        if (escaped) {
            // 处理转义字符
            switch (c) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '\\': value += '\\'; break;
                case '\'': value += '\''; break;
                case '"': value += '"'; break;
                default: value += c; break;
            }
            escaped = false;
            advance();
            ++column;
        } else if (c == '\\') {
            escaped = true;
            advance();
            ++column;
        } else if (c == quote) {
            advance(); // 跳过结束引号
            ++column;
            return Token(TokenType::STRING_LITERAL, value, start_line, start_column);
        } else if (c == '\n') {
            throw std::runtime_error(
                "Unterminated string literal at line " + std::to_string(start_line) + 
                ", column " + std::to_string(start_column)
            );
        } else {
            value += c;
            advance();
            ++column;
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
    if (position >= input.length()) {
        return '\0';
    }
    return input[position];
}

char Lexer::advance()
{
    if (position >= input.length()) {
        return '\0';
    }
    return input[position++];
}

bool Lexer::match(char expected)
{
    if (peek() == expected) {
        advance();
        ++column;
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

    if (upper_keyword == "SELECT") return TokenType::SELECT;
    if (upper_keyword == "INSERT") return TokenType::INSERT;
    if (upper_keyword == "DELETE") return TokenType::DELETE;
    if (upper_keyword == "UPDATE") return TokenType::UPDATE;
    if (upper_keyword == "CREATE") return TokenType::CREATE;
    if (upper_keyword == "DROP") return TokenType::DROP;
    if (upper_keyword == "FROM") return TokenType::FROM;
    if (upper_keyword == "WHERE") return TokenType::WHERE;
    if (upper_keyword == "INTO") return TokenType::INTO;
    if (upper_keyword == "VALUES") return TokenType::VALUES;
    if (upper_keyword == "SET") return TokenType::SET;
    if (upper_keyword == "AND") return TokenType::AND;
    if (upper_keyword == "OR") return TokenType::OR;
    if (upper_keyword == "NOT") return TokenType::NOT;
    if (upper_keyword == "AS") return TokenType::AS;
    if (upper_keyword == "COLLECTION") return TokenType::COLLECTION;
    if (upper_keyword == "INDEX") return TokenType::INDEX;
    if (upper_keyword == "TRUE") return TokenType::BOOLEAN_LITERAL;
    if (upper_keyword == "FALSE") return TokenType::BOOLEAN_LITERAL;
    if (upper_keyword == "NULL") return TokenType::NULL_LITERAL;

    return TokenType::IDENTIFIER;
}

} // namespace dreamdb
