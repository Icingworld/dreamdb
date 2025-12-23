#pragma once

#include <string>
#include <cstdint>

namespace dreamdb
{

/**
 * @brief Token 类型
 * @details 词法分析器会将 SQL 字符串分解为不同类型的 Token
 */
enum class TokenType : std::uint8_t
{
    // 结束标记
    DB_EOF_TOKEN,          // 文件结束（字符串结束）

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
    DB_ADD,                // ADD
    DB_MODIFY,             // MODIFY
    DB_RENAME,             // RENAME
    DB_COLUMN,             // COLUMN
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
    DB_TO,                 // TO
    DB_OFFSET,             // OFFSET
    DB_DATABASE,           // DATABASE
    DB_COLLECTION,         // COLLECTION
    DB_INDEX,              // INDEX
    DB_VINDEX,             // VINDEX
    DB_DATABASES,          // DATABASES
    DB_COLLECTIONS,        // COLLECTIONS
    DB_INDEXES,            // INDEXES
    DB_VINDEXES,           // VINDEXES
    DB_LIMIT,              // LIMIT
    DB_PRIMARY,            // PRIMARY
    DB_KEY,                // KEY
    DB_AUTO_INCREMENT,     // AUTO_INCREMENT
    DB_DEFAULT,            // DEFAULT
    DB_UNIQUE,             // UNIQUE
    DB_IF,                 // IF
    DB_EXISTS,             // EXISTS
    DB_ORDER,              // ORDER
    DB_BY,                 // BY
    DB_ASC,                // ASC
    DB_BETWEEN,            // BETWEEN
    DB_IN,                 // IN
    DB_LIKE,               // LIKE
    DB_IS,                 // IS
    DB_SEARCH,             // SEARCH
    DB_USING,              // USING
    DB_WITH,               // WITH
    DB_NULL,               // NULL
    DB_TRUE,               // TRUE
    DB_FALSE,              // FALSE
    DB_BTREE,              // BTREE
    DB_HASH,               // HASH
    DB_FLAT,               // FLAT
    DB_IVF_FLAT,           // IVF_FLAT
    DB_HNSW,               // HNSW

    // 标识符
    DB_IDENTIFIER,         // 表名、字段名、数据类型等（如 users, id, tinyint(1), varchar(255)）
    DB_STRING_LITERAL,     // 字符串字面量（如 'hello'）
    DB_NUMBER_LITERAL,     // 数字字面量（如 123, 45.6）

    // 运算符
    DB_EQUAL,              // =
    DB_NOT_EQUAL,          // !=
    DB_LESS_THAN,          // <
    DB_GREATER_THAN,       // >
    DB_LESS_EQUAL,         // <=
    DB_GREATER_EQUAL,      // >=
    DB_PLUS,               // +
    DB_MINUS,              // -
    DB_MULTIPLY,           // *
    DB_DIVIDE,             // /
    DB_MODULO,             // %

    // 分隔符
    DB_COMMA,              // ,
    DB_SEMICOLON,          // ;
    DB_DOT,                // .
    DB_LEFT_PAREN,         // (
    DB_RIGHT_PAREN,        // )
    DB_LEFT_BRACKET,       // [
    DB_RIGHT_BRACKET,      // ]
    DB_LEFT_BRACE,         // {
    DB_RIGHT_BRACE,        // }
};

/**
 * @brief Token 类
 * @details 表示一个词法单元，包含类型和值
 */
class Token
{
public:
    Token() = delete;

    /**
     * @brief 构造函数
     * @param type Token 类型
     * @param value Token 的值，对于关键字为空字符串
     * @param line 所在行号
     * @param column 所在列号
     */
    Token(TokenType type, const std::string & value = "", std::size_t line = 0, std::size_t column = 0);

    Token(const Token &) = default;

    Token(Token &&) noexcept = default;

    Token & operator=(const Token &) = default;

    Token & operator=(Token &&) noexcept = default;

    ~Token() = default;

public:
    /**
     * @brief 获取 Token 类型
     */
    TokenType get_type() const noexcept;

    /**
     * @brief 获取 Token 值
     */
    const std::string & get_value() const noexcept;

    /**
     * @brief 获取行号
     */
    std::size_t get_line() const noexcept;

    /**
     * @brief 获取列号
     */
    std::size_t get_column() const noexcept;

    /**
     * @brief 检查是否为关键字
     */
    bool is_keyword() const noexcept;

    /**
     * @brief 检查是否为运算符
     */
    bool is_operator() const noexcept;

    /**
     * @brief 转换为字符串
     */
    std::string to_string() const;

private:
    TokenType type_;          // Token 类型
    std::string value_;       // Token 值
    std::size_t line_;        // 所在行号
    std::size_t column_;      // 所在列号
};

} // namespace dreamdb
