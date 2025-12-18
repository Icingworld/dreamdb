#include "dreamdb/parser/token.h"

namespace dreamdb
{

Token::Token(TokenType type, const std::string & value, std::size_t line, std::size_t column)
    : type(type)
    , value(value)
    , line(line)
    , column(column)
{
}

TokenType Token::get_type() const noexcept
{
    return type;
}

const std::string & Token::get_value() const noexcept
{
    return value;
}

std::size_t Token::get_line() const noexcept
{
    return line;
}

std::size_t Token::get_column() const noexcept
{
    return column;
}

bool Token::is_keyword() const noexcept
{
    return type >= TokenType::SELECT && type <= TokenType::COLLECTION;
}

bool Token::is_operator() const noexcept
{
    return type >= TokenType::EQUAL && type <= TokenType::MODULO;
}

std::string Token::to_string() const
{
    std::string result = "Token(";
    
    // 类型名称
    switch (type) {
        // 结束标记
        case TokenType::EOF_TOKEN:
            result += "EOF";
            break;

        // 关键字
        case TokenType::SELECT:
            result += "SELECT";
            break;
        case TokenType::INSERT:
            result += "INSERT";
            break;
        case TokenType::DELETE_:
            result += "DELETE";
            break;
        case TokenType::UPDATE:
            result += "UPDATE";
            break;
        case TokenType::CREATE:
            result += "CREATE";
            break;
        case TokenType::DROP:
            result += "DROP";
            break;
        case TokenType::USE:
            result += "USE";
            break;
        case TokenType::FROM:
            result += "FROM";
            break;
        case TokenType::WHERE:
            result += "WHERE";
            break;
        case TokenType::INTO:
            result += "INTO";
            break;
        case TokenType::VALUES:
            result += "VALUES";
            break;
        case TokenType::SET:
            result += "SET";
            break;
        case TokenType::AND:
            result += "AND";
            break;
        case TokenType::OR:
            result += "OR";
            break;
        case TokenType::NOT:
            result += "NOT";
            break;
        case TokenType::AS:
            result += "AS";
            break;
        case TokenType::DATABASE:
            result += "DATABASE";
            break;
        case TokenType::COLLECTION:
            result += "COLLECTION";
            break;
        case TokenType::INDEX:
            result += "INDEX";
            break;
        case TokenType::LIMIT:
            result += "LIMIT";
            break;
        case TokenType::PRIMARY:
            result += "PRIMARY";
            break;
        case TokenType::KEY:
            result += "KEY";
            break;
        case TokenType::AUTO_INCREMENT:
            result += "AUTO_INCREMENT";
            break;
        case TokenType::DEFAULT:
            result += "DEFAULT";
            break;
        // 标识符和字面量
        case TokenType::IDENTIFIER:
            result += "IDENTIFIER";
            break;
        case TokenType::STRING_LITERAL:
            result += "STRING_LITERAL";
            break;
        case TokenType::NUMBER_LITERAL:
            result += "NUMBER_LITERAL";
            break;
        case TokenType::BOOLEAN_LITERAL:
            result += "BOOLEAN_LITERAL";
            break;

        // 运算符
        case TokenType::EQUAL:
            result += "=";
            break;
        case TokenType::NOT_EQUAL:
            result += "!=";
            break;
        case TokenType::LESS_THAN:
            result += "<";
            break;
        case TokenType::GREATER_THAN:
            result += ">";
            break;
        case TokenType::LESS_EQUAL:
            result += "<=";
            break;
        case TokenType::GREATER_EQUAL:
            result += ">=";
            break;
        case TokenType::PLUS:
            result += "+";
            break;
        case TokenType::MINUS:
            result += "-";
            break;
        case TokenType::MULTIPLY:
            result += "*";
            break;
        case TokenType::DIVIDE:
            result += "/";
            break;
        case TokenType::MODULO:
            result += "%";
            break;

        // 分隔符
        case TokenType::COMMA:
            result += ",";
            break;
        case TokenType::SEMICOLON:
            result += ";";
            break;
        case TokenType::DOT:
            result += ".";
            break;
        case TokenType::LEFT_PAREN:
            result += "(";
            break;
        case TokenType::RIGHT_PAREN:
            result += ")";
            break;
        case TokenType::LEFT_BRACKET:
            result += "[";
            break;
        case TokenType::RIGHT_BRACKET:
            result += "]";
            break;
        case TokenType::LEFT_BRACE:
            result += "{";
            break;
        case TokenType::RIGHT_BRACE:
            result += "}";
            break;
        // 特殊值
        case TokenType::NULL_LITERAL:
            result += "NULL";
            break;
        default:
            result += "UNKNOWN";
    }
    
    if (!value.empty()) {
        result += ", value='" + value + "'";
    }
    
    result += ", line=" + std::to_string(line) + ", column=" + std::to_string(column) + ")";
    return result;
}

} // namespace dreamdb
