#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <stdexcept>

#include "dreamdb/parser/lexer.h"
#include "dreamdb/parser/token.h"

namespace dreamdb
{

// 前向声明 AST 语句节点和表达式节点
class AstStatementNode;
class AstExpressionNode;

/**
 * @brief 语法分析异常
 */
class ParseException : public std::runtime_error
{
public:
    ParseException(const std::string & message, std::size_t line, std::size_t column);

    ParseException(const ParseException &) = default;

    ParseException(ParseException &&) noexcept = default;

    ParseException & operator=(const ParseException &) = default;

    ParseException & operator=(ParseException &&) noexcept = default;

    ~ParseException() noexcept override = default;

public:
    /**
     * @brief 获取行号
     * @return 行号
     */
    std::size_t get_line() const noexcept;

    /**
     * @brief 获取列号
     * @return 列号
     */
    std::size_t get_column() const noexcept;

    /**
     * @brief 获取错误消息
     * @return 错误消息
     */
    std::string get_message() const noexcept;

private:
    std::size_t line_;
    std::size_t column_;
};

/**
 * @brief 语法分析器
 * @details 将 Token 序列转换为 AST
 */
class Parser
{
public:
    explicit Parser(const std::string & input);

    explicit Parser(std::unique_ptr<Lexer> lexer);

    Parser(const Parser &) = delete;

    Parser(Parser &&) noexcept = default;

    Parser & operator=(const Parser &) = delete;

    Parser & operator=(Parser &&) noexcept = default;

    ~Parser() noexcept = default;

public:
    /**
     * @brief 解析 SQL 语句
     * @return AST 语句节点
     * @throws ParseException 如果解析失败
     */
    std::unique_ptr<AstStatementNode> parse();

private:
    // ========== 语句解析 ==========

    /**
     * @brief 解析单个语句
     * @return 语句节点
     */
    std::unique_ptr<AstStatementNode> parse_statement();

    /**
     * @brief 解析 SELECT 语句
     */
    std::unique_ptr<AstStatementNode> parse_select_statement();

    /**
     * @brief 解析 INSERT 语句
     */
    std::unique_ptr<AstStatementNode> parse_insert_statement();

    /**
     * @brief 解析 UPDATE 语句
     */
    std::unique_ptr<AstStatementNode> parse_update_statement();

    /**
     * @brief 解析 DELETE 语句
     */
    std::unique_ptr<AstStatementNode> parse_delete_statement();

    /**
     * @brief 解析 CREATE 语句
     */
    std::unique_ptr<AstStatementNode> parse_create_statement();

    /**
     * @brief 解析 DROP 语句
     */
    std::unique_ptr<AstStatementNode> parse_drop_statement();

    /**
     * @brief 解析 USE 语句
     */
    std::unique_ptr<AstStatementNode> parse_use_statement();

    /**
     * @brief 解析 ALTER 语句
     */
    std::unique_ptr<AstStatementNode> parse_alter_statement();

    /**
     * @brief 解析 SHOW 语句
     */
    std::unique_ptr<AstStatementNode> parse_show_statement();

    /**
     * @brief 解析 DESCRIBE 语句
     */
    std::unique_ptr<AstStatementNode> parse_describe_statement();

    // ========== 表达式解析 ==========

    /**
     * @brief 解析表达式（顶层入口）
     * 表达式优先级从低到高：
     * - OR (最低优先级)
     * - AND
     * - 比较运算符 (=, !=, <, >, <=, >=)
     * - 算术运算符 (+, -)
     * - 乘除运算符 (*, /, %)
     * - 一元运算符 (NOT, -, +)
     * - 函数调用、标识符、字面量 (最高优先级)
     */
    std::unique_ptr<AstExpressionNode> parse_expression();

    // ========== 辅助方法 ==========

    /**
     * @brief 获取下一个 Token
     */
    Token advance();

    /**
     * @brief 检查当前 Token 类型，匹配成功则消耗
     */
    bool match(TokenType type);

    /**
     * @brief 检查当前 Token 类型（不消耗）
     */
    bool check(TokenType type) const;

    /**
     * @brief 消耗当前 Token，如果类型不匹配则抛出异常
     * @param type 期望的 Token 类型
     * @param message 错误消息
     */
    void consume(TokenType type, const std::string & message);

    /**
     * @brief 检查是否为表达式结束符
     */
    bool is_expression_terminator() const;

    /**
     * @brief 跳过分号（如果存在）
     */
    void skip_semicolon();

    /**
     * @brief 抛出解析异常
     */
    [[noreturn]] void error(const std::string & message);

private:
    std::unique_ptr<Lexer> lexer_;      // 词法分析器
    Token current_token_;               // 当前 Token
};

} // namespace dreamdb
