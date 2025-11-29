#pragma once

#include <string>
#include <vector>

#include "dreamdb/parser/token.h"

namespace dreamdb
{

/**
 * @brief 词法分析器
 * @details 将 SQL 字符串分解为 Token 序列
 */
class Lexer
{
public:
    /**
     * @brief 构造函数
     * @param input SQL 字符串
     */
    explicit Lexer(const std::string & input);

    ~Lexer() = default;

public:
    /**
     * @brief 获取下一个 Token
     * @return 下一个 Token，如果到达末尾返回 EOF_TOKEN
     */
    Token next_token();

    /**
     * @brief 查看下一个 Token（不移动位置）
     * @return 下一个 Token
     */
    Token peek_token();

    /**
     * @brief 检查是否还有更多 Token
     * @return 如果还有 Token 返回 true
     */
    bool has_more() const noexcept;

    /**
     * @brief 获取当前位置
     * @return 当前位置（字符索引）
     */
    std::size_t get_position() const noexcept;

    /**
     * @brief 获取当前行号
     * @return 行号
     */
    std::size_t get_line() const noexcept;

    /**
     * @brief 获取当前列号
     * @return 列号
     */
    std::size_t get_column() const noexcept;

private:
    /**
     * @brief 跳过空白字符（空格、制表符、换行等）
     */
    void skip_whitespace();

    /**
     * @brief 读取一个标识符或关键字
     * @return Token
     */
    Token read_identifier_or_keyword();

    /**
     * @brief 读取一个数字
     * @return Token
     */
    Token read_number();

    /**
     * @brief 读取一个字符串字面量（用单引号或双引号包围）
     * @return Token
     */
    Token read_string();

    /**
     * @brief 检查是否为字母或下划线
     */
    bool is_alpha(char c) const noexcept;

    /**
     * @brief 检查是否为数字
     */
    bool is_digit(char c) const noexcept;

    /**
     * @brief 检查是否为字母、数字或下划线
     */
    bool is_alnum(char c) const noexcept;

    /**
     * @brief 获取当前字符（不移动位置）
     */
    char peek() const;

    /**
     * @brief 获取当前字符并移动到下一个
     */
    char advance();

    /**
     * @brief 检查当前字符是否匹配，如果匹配则前进
     */
    bool match(char expected);

    /**
     * @brief 将关键字字符串转换为 TokenType
     */
    TokenType keyword_to_token_type(const std::string & keyword) const;

private:
    std::string input;          // 输入字符串
    std::size_t position;       // 当前位置
    std::size_t line;           // 当前行号
    std::size_t column;         // 当前列号
    Token peeked_token;         // 预读的 Token
    bool has_peeked;            // 是否已经预读
};

} // namespace dreamdb

