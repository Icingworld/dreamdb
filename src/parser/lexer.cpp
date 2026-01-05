#include "dreamdb/parser/lexer.h"

#include <cctype>
#include <unordered_map>
#include <iterator>
#include <algorithm>

namespace dreamdb
{

namespace // anonymous namespace
{

// 预初始化关键字表
const std::unordered_map<std::string, TokenType> KEYWORD_MAP = {
    {"SELECT", TokenType::DB_SELECT},
    {"INSERT", TokenType::DB_INSERT},
    {"DELETE", TokenType::DB_DELETE},
    {"UPDATE", TokenType::DB_UPDATE},
    {"CREATE", TokenType::DB_CREATE},
    {"DROP", TokenType::DB_DROP},
    {"USE", TokenType::DB_USE},
    {"ALTER", TokenType::DB_ALTER},
    {"SHOW", TokenType::DB_SHOW},
    {"DESCRIBE", TokenType::DB_DESCRIBE},
    {"DESC", TokenType::DB_DESC},
    {"DATABASE", TokenType::DB_DATABASE},
    {"COLLECTION", TokenType::DB_COLLECTION},
    {"INDEX", TokenType::DB_INDEX},
    {"VINDEX", TokenType::DB_VINDEX},
    {"DATABASES", TokenType::DB_DATABASES},
    {"COLLECTIONS", TokenType::DB_COLLECTIONS},
    {"INDEXES", TokenType::DB_INDEXES},
    {"VINDEXES", TokenType::DB_VINDEXES},
    {"PRIMARY", TokenType::DB_PRIMARY},
    {"KEY", TokenType::DB_KEY},
    {"UNIQUE", TokenType::DB_UNIQUE},
    {"AUTO_INCREMENT", TokenType::DB_AUTO_INCREMENT},
    {"DEFAULT", TokenType::DB_DEFAULT},
    {"COMMENT", TokenType::DB_COMMENT},
    {"USING", TokenType::DB_USING},
    {"WITH", TokenType::DB_WITH},
    {"FROM", TokenType::DB_FROM},
    {"WHERE", TokenType::DB_WHERE},
    {"INTO", TokenType::DB_INTO},
    {"VALUES", TokenType::DB_VALUES},
    {"SET", TokenType::DB_SET},
    {"AND", TokenType::DB_AND},
    {"OR", TokenType::DB_OR},
    {"NOT", TokenType::DB_NOT},
    {"AS", TokenType::DB_AS},
    {"ON", TokenType::DB_ON},
    {"IF", TokenType::DB_IF},
    {"EXISTS", TokenType::DB_EXISTS},
    {"NULL", TokenType::DB_NULL},
    {"TRUE", TokenType::DB_TRUE},
    {"FALSE", TokenType::DB_FALSE},
};

} // anonymous namespace

Lexer::Lexer(const std::string & input)
    : input_(input)
    , position_(0)
    , line_(1)
    , column_(1)
    , peeked_token_(std::nullopt)
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
    // 如果已经预读，直接返回预读的 Token 并清空预读
    if (peeked_token_.has_value()) {
        Token token = peeked_token_.value();
        peeked_token_ = std::nullopt;
        return token;
    }

    return next_token_internal();
}

const Token & Lexer::peek_token()
{
    if (!peeked_token_.has_value()) {
        peeked_token_ = next_token_internal();
    }

    return peeked_token_.value();
}

bool Lexer::has_more() const noexcept
{
    return position_ < input_.length();
}

Token Lexer::next_token_internal()
{
    // 跳过空白字符
    skip_whitespace();

    // 检查是否到达末尾
    if (position_ >= input_.length()) {
        return Token(TokenType::DB_EOF, std::nullopt, line_, column_);
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
                // !=
                return Token(TokenType::DB_NOT_EQUAL, "!=", start_line, start_column);
            }
            return Token(TokenType::DB_NOT, "!", start_line, start_column);
        case '<':
            advance();
            if (match('=')) {
                // <=
                return Token(TokenType::DB_LESS_EQUAL, "<=", start_line, start_column);
            }
            if (match('>')) {
                // <>
                return Token(TokenType::DB_NOT_EQUAL, "<>", start_line, start_column);
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
            return Token(TokenType::DB_STAR, "*", start_line, start_column);
        case '/':
            advance();
            return Token(TokenType::DB_SLASH, "/", start_line, start_column);
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
        default:
            // 未知字符
            char unknown = advance();
            return Token(TokenType::DB_ERROR, std::string(1, unknown), start_line, start_column);
    }
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

    // 支持科学计数法（可选）
    if (position_ < input_.length() && (peek() == 'e' || peek() == 'E')) {
        std::size_t save_pos = position_;
        std::size_t save_column = column_;
        
        advance(); // 跳过 e 或 E
        ++column_;
        
        // 可选的符号
        if (peek() == '+' || peek() == '-') {
            advance();
            ++column_;
        }
        
        // 必须有数字
        if (is_digit(peek())) {
            while (position_ < input_.length() && is_digit(peek())) {
                advance();
                ++column_;
            }
        } else {
            // 不是科学计数法，回退
            position_ = save_pos;
            column_ = save_column;
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
                case 'n':
                    value += '\n'; // 换行
                    break;
                case 't':
                    value += '\t'; // 制表符
                    break;
                case 'r':
                    value += '\r'; // 回车
                    break;
                case 'b':
                    value += '\b'; // 退格
                    break;
                case 'f':
                    value += '\f'; // 换页
                    break;
                case 'v':
                    value += '\v'; // 垂直制表符
                    break;
                case '0':
                    value += '\0'; // 空字符
                    break;
                case '\\':
                    value += '\\'; // 反斜杠
                    break;
                case '\'':
                    value += '\''; // 单引号
                    break;
                case '"':
                    value += '"';  // 双引号
                    break;
                default:
                    value += c;    // 其他字符
                    break;
            }
            escaped = false;
            advance();
            ++column_;  // 转义序列占两个字符位置（\ 和转义字符），但 \ 的列号已在之前更新
        } else if (c == '\\') {
            escaped = true;
            advance();
            ++column_;  // \ 字符的列号
        } else if (c == quote) {
            advance(); // 跳过结束引号
            ++column_;
            return Token(TokenType::DB_STRING_LITERAL, value, start_line, start_column);
        } else if (c == '\n' || c == '\r') {
            return Token(TokenType::DB_ERROR, "Unterminated string literal", start_line, start_column);
        } else {
            value += c;
            advance();
            ++column_;
        }
    }

    return Token(TokenType::DB_ERROR, "Unterminated string literal", start_line, start_column);
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
    std::string upper_keyword;
    upper_keyword.reserve(keyword.size());  // 预分配空间
    
    std::transform(keyword.begin(), keyword.end(),  std::back_inserter(upper_keyword), 
        [](unsigned char c) {
            return static_cast<char>(std::toupper(c));
        }
    );

    auto it = KEYWORD_MAP.find(upper_keyword);
    if (it != KEYWORD_MAP.end()) {
        return it->second;
    }

    return TokenType::DB_IDENTIFIER;
}

} // namespace dreamdb
