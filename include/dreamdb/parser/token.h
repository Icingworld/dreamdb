#pragma once

#include <cstdint>
#include <string>
#include <optional>

namespace dreamdb
{

/**
 * @brief Token 类型
 * @details 词法分析器会将 SQL 字符串分解为不同类型的 Token
 */
enum class TokenType : std::uint8_t
{
    // 结束标记
    DB_EOF,                // 结束标记

    // 关键字
    DB_SELECT,             // SELECT
    DB_INSERT,             // INSERT
    DB_DELETE,             // DELETE
    DB_UPDATE,             // UPDATE
    DB_CREATE,             // CREATE
    DB_DROP,               // DROP
    DB_USE,                // USE
    DB_ALTER,              // ALTER
    DB_SHOW,               // SHOW
    DB_DESCRIBE,           // DESCRIBE
    DB_DESC,               // DESC

    DB_DATABASE,           // DATABASE
    DB_COLLECTION,         // COLLECTION
    DB_INDEX,              // INDEX
    DB_VINDEX,             // VINDEX
    DB_DATABASES,          // DATABASES
    DB_COLLECTIONS,        // COLLECTIONS
    DB_INDEXES,            // INDEXES
    DB_VINDEXES,           // VINDEXES
    DB_GROUP,              // GROUP
    DB_BY,                 // BY
    DB_HAVING,             // HAVING
    DB_ORDER,              // ORDER
    DB_ASC,                // ASC
    DB_LIMIT,              // LIMIT
    DB_OFFSET,             // OFFSET
    DB_IN,                 // IN
    DB_BETWEEN,            // BETWEEN
    DB_LIKE,               // LIKE

    DB_ADD,                // ADD
    DB_MODIFY,             // MODIFY
    DB_RENAME,             // RENAME
    DB_COLUMN,             // COLUMN
    DB_TO,                 // TO
    DB_PRIMARY,            // PRIMARY
    DB_KEY,                // KEY
    DB_UNIQUE,             // UNIQUE
    DB_AUTO_INCREMENT,     // AUTO_INCREMENT
    DB_DEFAULT,            // DEFAULT
    DB_COMMENT,            // COMMENT
    DB_USING,              // USING
    DB_WITH,               // WITH
    DB_FROM,               // FROM
    DB_WHERE,              // WHERE
    DB_INTO,               // INTO
    DB_VALUES,             // VALUES
    DB_SET,                // SET
    DB_AND,                // AND
    DB_OR,                 // OR
    DB_NOT,                // NOT
    DB_AS,                 // AS
    DB_ON,                 // ON
    DB_IF,                 // IF
    DB_EXISTS,             // EXISTS
    DB_NULL,               // NULL
    DB_TRUE,               // TRUE
    DB_FALSE,              // FALSE

    // 标识符
    DB_IDENTIFIER,         // 表名、字段名、数据类型等
    DB_STRING_LITERAL,     // 字符串字面量
    DB_INTEGER_LITERAL,    // 整数字面量
    DB_FLOAT_LITERAL,      // 浮点数字面量

    // 运算符
    DB_EQUAL,              // =
    DB_NOT_EQUAL,          // != 和 <>
    DB_LESS_THAN,          // <
    DB_GREATER_THAN,       // >
    DB_LESS_EQUAL,         // <=
    DB_GREATER_EQUAL,      // >=
    DB_PLUS,               // +
    DB_MINUS,              // -
    DB_STAR,               // *
    DB_SLASH,              // /
    DB_MODULO,             // %

    /** 分隔符 */
    DB_COMMA,              // ,
    DB_SEMICOLON,          // ;
    DB_DOT,                // .
    DB_LEFT_PAREN,         // (
    DB_RIGHT_PAREN,        // )
    DB_LEFT_BRACKET,       // [
    DB_RIGHT_BRACKET,      // ]

    // 错误
    DB_ERROR               // 错误
};

/**
 * @brief Token 类
 * @details 表示一个词法单元，包含类型和值
 */
class Token
{
public:
    Token(TokenType type, std::optional<std::string> value = std::nullopt, std::size_t line = 0, std::size_t column = 0);

    Token(const Token &) = default;

    Token(Token &&) noexcept = default;

    Token & operator=(const Token &) = default;

    Token & operator=(Token &&) noexcept = default;

    ~Token() noexcept = default;

public:
    /**
     * @brief 获取 Token 类型
     * @return Token 类型
     */
    TokenType get_type() const noexcept;

    /**
     * @brief 获取 Token 值
     * @return Token 值
     */
    const std::string & get_value() const;

    /**
     * @brief 获取行号
     * @return 所在行号
     */
    std::size_t get_line() const noexcept;

    /**
     * @brief 获取列号
     * @return 所在列号
     */
    std::size_t get_column() const noexcept;

    /**
     * @brief 是否存在值
     * @return 是否存在值
     */
    bool has_value() const noexcept;

private:
    TokenType type_;                    // Token 类型
    std::optional<std::string> value_;  // Token 值，仅仅在标识符类型时有效
    std::size_t line_;                  // 所在行号
    std::size_t column_;                // 所在列号
};

} // namespace dreamdb
