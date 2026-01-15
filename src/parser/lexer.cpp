#include "dreamdb/parser/lexer.h"

#include <cctype>
#include <unordered_map>
#include <iterator>
#include <algorithm>

namespace dreamdb::parser
{

namespace // anonymous namespace
{

// 预初始化关键字表
// 该表将在运行时初始化
const std::unordered_map<std::string, TokenType> KEYWORD_MAP = {
    {"SELECT", TokenType::Select},
    {"INSERT", TokenType::Insert},
    {"DELETE", TokenType::Delete},
    {"UPDATE", TokenType::Update},
    {"CREATE", TokenType::Create},
    {"DROP", TokenType::Drop},
    {"USE", TokenType::Use},
    {"ALTER", TokenType::Alter},
    {"SHOW", TokenType::Show},
    {"DESCRIBE", TokenType::Describe},
    {"DESC", TokenType::Desc},
    {"DATABASE", TokenType::Database},
    {"COLLECTION", TokenType::Collection},
    {"INDEX", TokenType::Index},
    {"VINDEX", TokenType::VIndex},
    {"DATABASES", TokenType::Databases},
    {"COLLECTIONS", TokenType::Collections},
    {"INDEXES", TokenType::Indexes},
    {"VINDEXES", TokenType::VIndexes},
    {"GROUP", TokenType::Group},
    {"BY", TokenType::By},
    {"HAVING", TokenType::Having},
    {"ORDER", TokenType::Order},
    {"ASC", TokenType::Asc},
    {"LIMIT", TokenType::Limit},
    {"OFFSET", TokenType::Offset},
    {"IN", TokenType::In},
    {"BETWEEN", TokenType::Between},
    {"LIKE", TokenType::Like},
    {"ADD", TokenType::Add},
    {"MODIFY", TokenType::Modify},
    {"RENAME", TokenType::Rename},
    {"COLUMN", TokenType::Column},
    {"TO", TokenType::To},
    {"PRIMARY", TokenType::Primary},
    {"KEY", TokenType::Key},
    {"UNIQUE", TokenType::Unique},
    {"AUTO_INCREMENT", TokenType::AutoIncrement},
    {"DEFAULT", TokenType::Default},
    {"COMMENT", TokenType::Comment},
    {"USING", TokenType::Using},
    {"WITH", TokenType::With},
    {"FROM", TokenType::From},
    {"WHERE", TokenType::Where},
    {"INTO", TokenType::Into},
    {"VALUES", TokenType::Values},
    {"SET", TokenType::Set},
    {"AND", TokenType::And},
    {"OR", TokenType::Or},
    {"NOT", TokenType::Not},
    {"AS", TokenType::As},
    {"ON", TokenType::On},
    {"IF", TokenType::If},
    {"EXISTS", TokenType::Exists},
    {"NULL", TokenType::Null},
    {"TRUE", TokenType::True},
    {"FALSE", TokenType::False},
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

std::size_t Lexer::line() const noexcept
{
    return line_;
}

std::size_t Lexer::column() const noexcept
{
    return column_;
}

Token Lexer::next()
{
    // 如果已经预读，直接返回预读的 Token 并清空预读
    if (peeked_token_.has_value()) {
        Token token = peeked_token_.value();
        peeked_token_ = std::nullopt;
        return token;
    }

    return next_internal();
}

const Token & Lexer::peek()
{
    if (!peeked_token_.has_value()) {
        peeked_token_ = next_internal();
    }

    return peeked_token_.value();
}

bool Lexer::has_more() const noexcept
{
    return position_ < input_.length();
}

Token Lexer::next_internal()
{
    // 跳过空白字符
    skip_whitespace();

    // 检查是否到达末尾
    if (position_ >= input_.length()) {
        return Token(TokenType::EoF, "", line_, column_);
    }

    char current = peek_char();
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
            return Token(TokenType::Equal, "=", start_line, start_column);
        case '!':
            advance();
            if (match('=')) {
                // !=
                return Token(TokenType::NotEqual, "!=", start_line, start_column);
            }
            // 不支持单独的 ! 符号
            return Token(TokenType::Error, std::string(1, current), start_line, start_column);
        case '<':
            advance();
            if (match('=')) {
                // <=
                return Token(TokenType::LessEqual, "<=", start_line, start_column);
            }
            if (match('>')) {
                // <>
                return Token(TokenType::NotEqual, "<>", start_line, start_column);
            }
            return Token(TokenType::LessThan, "<", start_line, start_column);
        case '>':
            advance();
            if (match('=')) {
                return Token(TokenType::GreaterEqual, ">=", start_line, start_column);
            }
            return Token(TokenType::GreaterThan, ">", start_line, start_column);
        case '+':
            advance();
            return Token(TokenType::Plus, "+", start_line, start_column);
        case '-':
            advance();
            return Token(TokenType::Minus, "-", start_line, start_column);
        case '*':
            advance();
            return Token(TokenType::Star, "*", start_line, start_column);
        case '/':
            advance();
            return Token(TokenType::Slash, "/", start_line, start_column);
        case '%':
            advance();
            return Token(TokenType::Modulo, "%", start_line, start_column);
        case ',':
            advance();
            return Token(TokenType::Comma, ",", start_line, start_column);
        case ';':
            advance();
            return Token(TokenType::Semicolon, ";", start_line, start_column);
        case '.':
            advance();
            return Token(TokenType::Dot, ".", start_line, start_column);
        case '(':
            advance();
            return Token(TokenType::LeftParen, "(", start_line, start_column);
        case ')':
            advance();
            return Token(TokenType::RightParen, ")", start_line, start_column);
        case '[':
            advance();
            return Token(TokenType::LeftBracket, "[", start_line, start_column);
        case ']':
            advance();
            return Token(TokenType::RightBracket, "]", start_line, start_column);
        default:
            // 未知字符
            char unknown = advance();
            return Token(TokenType::Error, std::string(1, unknown), start_line, start_column);
    }
}

void Lexer::skip_whitespace()
{
    while (position_ < input_.length()) {
        char c = peek_char();
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            advance();
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
    while (position_ < input_.length() && is_alnum(peek_char())) {
        advance();
    }

    std::string text = input_.substr(start, position_ - start);

    // 检查是否为关键字
    TokenType type = keyword_to_token_type(text);
    if (type != TokenType::Identifier) {
        return Token(type, text, start_line, start_column);
    }

    // 是标识符
    return Token(TokenType::Identifier, text, start_line, start_column);
}

Token Lexer::read_number()
{
    std::size_t start = position_;
    std::size_t start_line = line_;
    std::size_t start_column = column_;

    bool is_float = false;

    // 读取整数部分
    while (position_ < input_.length() && is_digit(peek_char())) {
        advance();
    }

    // 检查是否有小数点（浮点数）
    if (peek_char() == '.' && position_ + 1 < input_.length() && is_digit(input_[position_ + 1])) {
        is_float = true;
        advance(); // 跳过小数点

        // 读取小数部分
        while (position_ < input_.length() && is_digit(peek_char())) {
            advance();
        }
    }

    // 支持科学计数法（可选）
    if (position_ < input_.length() && (peek_char() == 'e' || peek_char() == 'E')) {
        std::size_t save_pos = position_;
        std::size_t save_column = column_;

        advance(); // 跳过 e 或 E

        // 可选的符号
        if (peek_char() == '+' || peek_char() == '-') {
            advance();
        }

        // 必须有数字
        if (is_digit(peek_char())) {
            is_float = true; // 科学计数法总是浮点数
            while (position_ < input_.length() && is_digit(peek_char())) {
                advance();
            }
        } else {
            // 不是科学计数法，回退
            position_ = save_pos;
            column_ = save_column;
        }
    }

    std::string text = input_.substr(start, position_ - start);
    TokenType type = is_float ? TokenType::FloatLiteral : TokenType::IntegerLiteral;
    return Token(type, text, start_line, start_column);
}

Token Lexer::read_string()
{
    std::size_t start_line = line_;
    std::size_t start_column = column_;
    char quote = advance(); // 跳过开始引号

    std::string value;
    bool escaped = false;

    while (position_ < input_.length()) {
        char c = peek_char();

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
        } else if (c == '\\') {
            escaped = true;
            advance();
        } else if (c == quote) {
            advance(); // 跳过结束引号
            return Token(TokenType::StringLiteral, value, start_line, start_column);
        } else if (c == '\n' || c == '\r') {
            return Token(TokenType::Error, "Unterminated string literal", start_line, start_column);
        } else {
            value += c;
            advance();
        }
    }

    return Token(TokenType::Error, "Unterminated string literal", start_line, start_column);
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

char Lexer::peek_char() const
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

    char c = input_[position_++];

    if (c == '\n') {
        ++line_;
        column_ = 1;
    } else if (c == '\r') {
        // Windows CRLF：如果下一字符是 '\n'，一并吞掉
        if (position_ < input_.length() && input_[position_] == '\n') {
            ++position_;
        }
        ++line_;
        column_ = 1;
    } else {
        ++column_;
    }

    return c;
}

bool Lexer::match(char expected)
{
    if (peek_char() == expected) {
        advance();
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

    return TokenType::Identifier;
}

} // namespace dreamdb::parser
