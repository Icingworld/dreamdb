#include "dreamdb/parser/token.h"

namespace dreamdb
{

Token::Token(TokenType type, const std::string & value, std::size_t line, std::size_t column)
    : type_(type)
    , value_(value)
    , line_(line)
    , column_(column)
{
}

TokenType Token::get_type() const noexcept
{
    return type_;
}

const std::string & Token::get_value() const noexcept
{
    return value_;
}

std::size_t Token::get_line() const noexcept
{
    return line_;
}

std::size_t Token::get_column() const noexcept
{
    return column_;
}

bool Token::is_keyword() const noexcept
{
    return type_ >= TokenType::DB_SELECT && type_ <= TokenType::DB_WITH;
}

bool Token::is_operator() const noexcept
{
    return type_ >= TokenType::DB_EQUAL && type_ <= TokenType::DB_MODULO;
}

std::string Token::to_string() const
{
    std::string result = "Token(";
    
    // 类型名称
    switch (type_) {
        // 结束标记
        case TokenType::DB_EOF_TOKEN:
            result += "EOF";
            break;

        // 关键字
        case TokenType::DB_SELECT:
            result += "SELECT";
            break;
        case TokenType::DB_INSERT:
            result += "INSERT";
            break;
        case TokenType::DB_DELETE:
            result += "DELETE";
            break;
        case TokenType::DB_UPDATE:
            result += "UPDATE";
            break;
        case TokenType::DB_CREATE:
            result += "CREATE";
            break;
        case TokenType::DB_DROP:
            result += "DROP";
            break;
        case TokenType::DB_USE:
            result += "USE";
            break;
        case TokenType::DB_ALTER:
            result += "ALTER";
            break;
        case TokenType::DB_SHOW:
            result += "SHOW";
            break;
        case TokenType::DB_DESCRIBE:
            result += "DESCRIBE";
            break;
        case TokenType::DB_DESC:
            result += "DESC";
            break;
        case TokenType::DB_ADD:
            result += "ADD";
            break;
        case TokenType::DB_MODIFY:
            result += "MODIFY";
            break;
        case TokenType::DB_RENAME:
            result += "RENAME";
            break;
        case TokenType::DB_COLUMN:
            result += "COLUMN";
            break;
        case TokenType::DB_FROM:
            result += "FROM";
            break;
        case TokenType::DB_WHERE:
            result += "WHERE";
            break;
        case TokenType::DB_INTO:
            result += "INTO";
            break;
        case TokenType::DB_VALUES:
            result += "VALUES";
            break;
        case TokenType::DB_SET:
            result += "SET";
            break;
        case TokenType::DB_AND:
            result += "AND";
            break;
        case TokenType::DB_OR:
            result += "OR";
            break;
        case TokenType::DB_NOT:
            result += "NOT";
            break;
        case TokenType::DB_AS:
            result += "AS";
            break;
        case TokenType::DB_ON:
            result += "ON";
            break;
        case TokenType::DB_TO:
            result += "TO";
            break;
        case TokenType::DB_OFFSET:
            result += "OFFSET";
            break;
        case TokenType::DB_DATABASE:
            result += "DATABASE";
            break;
        case TokenType::DB_COLLECTION:
            result += "COLLECTION";
            break;
        case TokenType::DB_INDEX:
            result += "INDEX";
            break;
        case TokenType::DB_VINDEX:
            result += "VINDEX";
            break;
        case TokenType::DB_DATABASES:
            result += "DATABASES";
            break;
        case TokenType::DB_COLLECTIONS:
            result += "COLLECTIONS";
            break;
        case TokenType::DB_INDEXES:
            result += "INDEXES";
            break;
        case TokenType::DB_VINDEXES:
            result += "VINDEXES";
            break;
        case TokenType::DB_LIMIT:
            result += "LIMIT";
            break;
        case TokenType::DB_PRIMARY:
            result += "PRIMARY";
            break;
        case TokenType::DB_KEY:
            result += "KEY";
            break;
        case TokenType::DB_AUTO_INCREMENT:
            result += "AUTO_INCREMENT";
            break;
        case TokenType::DB_DEFAULT:
            result += "DEFAULT";
            break;
        case TokenType::DB_UNIQUE:
            result += "UNIQUE";
            break;
        case TokenType::DB_IF:
            result += "IF";
            break;
        case TokenType::DB_EXISTS:
            result += "EXISTS";
            break;
        case TokenType::DB_ORDER:
            result += "ORDER";
            break;
        case TokenType::DB_BY:
            result += "BY";
            break;
        case TokenType::DB_ASC:
            result += "ASC";
            break;
        case TokenType::DB_BETWEEN:
            result += "BETWEEN";
            break;
        case TokenType::DB_IN:
            result += "IN";
            break;
        case TokenType::DB_LIKE:
            result += "LIKE";
            break;
        case TokenType::DB_IS:
            result += "IS";
            break;
        case TokenType::DB_SEARCH:
            result += "SEARCH";
            break;
        case TokenType::DB_USING:
            result += "USING";
            break;
        case TokenType::DB_WITH:
            result += "WITH";
            break;
        case TokenType::DB_NULL:
            result += "NULL";
            break;
        case TokenType::DB_TRUE:
            result += "TRUE";
            break;
        case TokenType::DB_FALSE:
            result += "FALSE";
            break;
        case TokenType::DB_BTREE:
            result += "BTREE";
            break;
        case TokenType::DB_HASH:
            result += "HASH";
            break;
        case TokenType::DB_FLAT:
            result += "FLAT";
            break;
        case TokenType::DB_IVF_FLAT:
            result += "IVF_FLAT";
            break;
        case TokenType::DB_HNSW:
            result += "HNSW";
            break;

        // 标识符和字面量
        case TokenType::DB_IDENTIFIER:
            result += "IDENTIFIER";
            break;
        case TokenType::DB_STRING_LITERAL:
            result += "STRING_LITERAL";
            break;
        case TokenType::DB_NUMBER_LITERAL:
            result += "NUMBER_LITERAL";
            break;

        // 运算符
        case TokenType::DB_EQUAL:
            result += "=";
            break;
        case TokenType::DB_NOT_EQUAL:
            result += "!=";
            break;
        case TokenType::DB_LESS_THAN:
            result += "<";
            break;
        case TokenType::DB_GREATER_THAN:
            result += ">";
            break;
        case TokenType::DB_LESS_EQUAL:
            result += "<=";
            break;
        case TokenType::DB_GREATER_EQUAL:
            result += ">=";
            break;
        case TokenType::DB_PLUS:
            result += "+";
            break;
        case TokenType::DB_MINUS:
            result += "-";
            break;
        case TokenType::DB_MULTIPLY:
            result += "*";
            break;
        case TokenType::DB_DIVIDE:
            result += "/";
            break;
        case TokenType::DB_MODULO:
            result += "%";
            break;

        // 分隔符
        case TokenType::DB_COMMA:
            result += ",";
            break;
        case TokenType::DB_SEMICOLON:
            result += ";";
            break;
        case TokenType::DB_DOT:
            result += ".";
            break;
        case TokenType::DB_LEFT_PAREN:
            result += "(";
            break;
        case TokenType::DB_RIGHT_PAREN:
            result += ")";
            break;
        case TokenType::DB_LEFT_BRACKET:
            result += "[";
            break;
        case TokenType::DB_RIGHT_BRACKET:
            result += "]";
            break;
        case TokenType::DB_LEFT_BRACE:
            result += "{";
            break;
        case TokenType::DB_RIGHT_BRACE:
            result += "}";
            break;

        default:
            result += "UNKNOWN";
    }
    
    if (!value_.empty()) {
        result += ", value='" + value_ + "'";
    }
    
    result += ", line=" + std::to_string(line_) + ", column=" + std::to_string(column_) + ")";
    return result;
}

} // namespace dreamdb
