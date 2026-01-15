#pragma once

#include <cstdint>
#include <string>
#include <optional>

namespace dreamdb::parser
{

/**
 * @brief Token 类型
 * @details 词法分析器会将 SQL 字符串分解为不同类型的 Token
 */
enum class TokenType : std::uint8_t
{
    // 结束标记
    EoF,                // 结束标记

    // 关键字
    Select,             // SELECT
    Insert,             // INSERT
    Delete,             // DELETE
    Update,             // UPDATE
    Create,             // CREATE
    Drop,               // DROP
    Use,                // USE
    Alter,              // ALTER
    Show,               // SHOW
    Describe,           // DESCRIBE
    Desc,               // DESC

    Database,           // DATABASE
    Collection,         // COLLECTION
    Index,              // INDEX
    VIndex,             // VINDEX
    Databases,          // DATABASES
    Collections,        // COLLECTIONS
    Indexes,            // INDEXES
    VIndexes,           // VINDEXES
    Group,              // GROUP
    By,                 // BY
    Having,             // HAVING
    Order,              // ORDER
    Asc,                // ASC
    Limit,              // LIMIT
    Offset,             // OFFSET
    In,                 // IN
    Between,            // BETWEEN
    Like,               // LIKE

    Add,                // ADD
    Modify,             // MODIFY
    Rename,             // RENAME
    Column,             // COLUMN
    To,                 // TO
    Primary,            // PRIMARY
    Key,                // KEY
    Unique,             // UNIQUE
    AutoIncrement,      // AUTO_INCREMENT
    Default,            // DEFAULT
    Comment,            // COMMENT
    Using,              // USING
    With,               // WITH
    From,               // FROM
    Where,              // WHERE
    Into,               // INTO
    Values,             // VALUES
    Set,                // SET
    And,                // AND
    Or,                 // OR
    Not,                // NOT
    As,                 // AS
    On,                 // ON
    If,                 // IF
    Exists,             // EXISTS
    Null,               // NULL
    True,               // TRUE
    False,              // FALSE

    // 标识符
    Identifier,         // 表名、字段名、数据类型等
    StringLiteral,      // 字符串字面量
    IntegerLiteral,     // 整数字面量
    FloatLiteral,       // 浮点数字面量

    // 运算符
    Equal,              // =
    NotEqual,           // != 或 <>
    LessThan,           // <
    GreaterThan,        // >
    LessEqual,          // <=
    GreaterEqual,       // >=
    Plus,               // +
    Minus,              // -
    Star,               // *
    Slash,              // /
    Modulo,             // %

    /** 分隔符 */
    Comma,              // ,
    Semicolon,          // ;
    Dot,                // .
    LeftParen,          // (
    RightParen,         // )
    LeftBracket,        // [
    RightBracket,       // ]

    // 错误
    Error               // 错误
};

/**
 * @brief Token 类
 * @details 表示一个词法单元，包含类型和值
 */
class Token
{
public:
    Token(
        TokenType type,
        std::string value,
        std::size_t line = 0,
        std::size_t column = 0
    );

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
    TokenType type() const noexcept;

    /**
     * @brief 获取 Token 值
     * @return Token 值
     */
    const std::string & value() const noexcept;

    /**
     * @brief 获取行号
     * @return 所在行号
     */
    std::size_t line() const noexcept;

    /**
     * @brief 获取列号
     * @return 所在列号
     */
    std::size_t column() const noexcept;

private:
    TokenType type_;                    // Token 类型
    std::string value_;                 // Token 值
    std::size_t line_;                  // 所在行号
    std::size_t column_;                // 所在列号
};

} // namespace dreamdb::parser
