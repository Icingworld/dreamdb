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
    EOF_TOKEN,          // 文件结束（字符串结束）

    // 关键字
    SELECT,
    INSERT,
    DELETE,
    UPDATE,
    CREATE,
    DROP,
    USE,
    FROM,
    WHERE,
    INTO,
    VALUES,
    SET,
    AND,
    OR,
    NOT,
    AS,
    DATABASE,
    COLLECTION,
    INDEX,
    LIMIT,
    PRIMARY,
    KEY,
    AUTO_INCREMENT,
    DEFAULT,

    // 标识符
    IDENTIFIER,         // 表名、字段名等（如 users, id）
    STRING_LITERAL,     // 字符串字面量（如 'hello'）
    NUMBER_LITERAL,     // 数字字面量（如 123, 45.6）
    BOOLEAN_LITERAL,    // 布尔值（true, false）

    // 运算符
    EQUAL,              // =
    NOT_EQUAL,          // != 或 <>
    LESS_THAN,          // <
    GREATER_THAN,       // >
    LESS_EQUAL,         // <=
    GREATER_EQUAL,      // >=
    PLUS,               // +
    MINUS,              // -
    MULTIPLY,           // *
    DIVIDE,             // /
    MODULO,             // %

    // 分隔符
    COMMA,              // ,
    SEMICOLON,          // ;
    DOT,                // .
    LEFT_PAREN,         // (
    RIGHT_PAREN,        // )
    LEFT_BRACKET,       // [
    RIGHT_BRACKET,      // ]
    LEFT_BRACE,         // {
    RIGHT_BRACE,        // }

    // 特殊值
    NULL_LITERAL,       // NULL
};

/**
 * @brief Token 类
 * @details 表示一个词法单元，包含类型和值
 */
class Token
{
public:
    Token() = default;

    /**
     * @brief 构造函数
     * @param type Token 类型
     * @param value Token 的值，对于关键字为空字符串
     * @param line 所在行号
     * @param column 所在列号
     */
    Token(TokenType type, const std::string & value = "", std::size_t line = 0, std::size_t column = 0);

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
    TokenType type;          // Token 类型
    std::string value;       // Token 值
    std::size_t line;        // 所在行号
    std::size_t column;      // 所在列号
};

} // namespace dreamdb

