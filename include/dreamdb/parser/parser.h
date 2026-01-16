#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <stdexcept>

#include "dreamdb/parser/token.h"
#include "dreamdb/parser/ast/statement/column_definition.h"

namespace dreamdb::parser
{

class Lexer;

namespace ast
{

class AstStatement;
class AstExpression;

} // namespace ast

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
    std::size_t line() const noexcept;

    /**
     * @brief 获取列号
     * @return 列号
     */
    std::size_t column() const noexcept;

    /**
     * @brief 获取错误消息
     * @return 错误消息
     */
    std::string message() const noexcept;

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

    ~Parser() noexcept;

public:
    /**
     * @brief 解析 SQL 语句
     * @return AST 语句节点
     * @throws ParseException 如果解析失败
     */
    std::unique_ptr<ast::AstStatement> parse();

private:
    // ========== 语句解析 ==========

    /**
     * @brief 解析单个语句
     * @return 语句节点
     */
    std::unique_ptr<ast::AstStatement> parse_statement();

    /**
     * @brief 解析 SELECT 语句
     */
    std::unique_ptr<ast::AstStatement> parse_select_statement();

    /**
     * @brief 解析 INSERT 语句
     */
    std::unique_ptr<ast::AstStatement> parse_insert_statement();

    /**
     * @brief 解析 UPDATE 语句
     */
    std::unique_ptr<ast::AstStatement> parse_update_statement();

    /**
     * @brief 解析 DELETE 语句
     */
    std::unique_ptr<ast::AstStatement> parse_delete_statement();

    /**
     * @brief 解析 CREATE 语句
     */
    std::unique_ptr<ast::AstStatement> parse_create_statement();

    /**
     * @brief 解析 DROP 语句
     */
    std::unique_ptr<ast::AstStatement> parse_drop_statement();

    /**
     * @brief 解析 USE 语句
     */
    std::unique_ptr<ast::AstStatement> parse_use_statement();

    /**
     * @brief 解析 ALTER 语句
     */
    std::unique_ptr<ast::AstStatement> parse_alter_statement();

    /**
     * @brief 解析 SHOW 语句
     */
    std::unique_ptr<ast::AstStatement> parse_show_statement();

    /**
     * @brief 解析 DESCRIBE 语句
     */
    std::unique_ptr<ast::AstStatement> parse_describe_statement();

    // ========== 表达式解析 ==========

    /**
     * @brief 解析表达式（顶层入口）
     * @return 表达式节点
     */
    std::unique_ptr<ast::AstExpression> parse_expression();

    /**
     * @brief 解析或表达式
     * @return 或表达式节点
     */
    std::unique_ptr<ast::AstExpression> parse_or_expression();

    /**
     * @brief 解析与表达式
     * @return 与表达式节点
     */
    std::unique_ptr<ast::AstExpression> parse_and_expression();

    /**
     * @brief 解析比较表达式
     * @return 比较表达式节点
     */
    std::unique_ptr<ast::AstExpression> parse_comparison_expression();

    /**
     * @brief 解析加法表达式
     * @return 加法表达式节点
     */
    std::unique_ptr<ast::AstExpression> parse_additive_expression();

    /**
     * @brief 解析乘法表达式
     * @return 乘法表达式节点
     */
    std::unique_ptr<ast::AstExpression> parse_multiplicative_expression();

    /**
     * @brief 解析一元表达式
     * @return 一元表达式节点
     */
    std::unique_ptr<ast::AstExpression> parse_unary_expression();

    /**
     * @brief 解析主表达式
     * @return 主表达式节点
     */
    std::unique_ptr<ast::AstExpression> parse_primary_expression();

    /** ========== 表达式解析辅助方法 ========== */

    /**
     * @brief 解析单个列定义
     * @return 列定义
     */
    ast::AstColumnDefinition parse_column_definition();

    /**
     * @brief 解析 IN 表达式
     * @param left 左表达式
     * @param is_not 是否为 NOT IN
     * @return IN 表达式节点
     */
    std::unique_ptr<ast::AstExpression> parse_in_expression(std::unique_ptr<ast::AstExpression> left, bool is_not);

    /**
     * @brief 解析 BETWEEN 表达式
     * @param left 左表达式
     * @param is_not 是否为 NOT BETWEEN
     * @return BETWEEN 表达式节点
     */
    std::unique_ptr<ast::AstExpression> parse_between_expression(std::unique_ptr<ast::AstExpression> left, bool is_not);

    /**
     * @brief 解析 LIKE 表达式
     * @param left 左表达式
     * @param is_not 是否为 NOT LIKE
     * @return LIKE 表达式节点
     */
    std::unique_ptr<ast::AstExpression> parse_like_expression(std::unique_ptr<ast::AstExpression> left, bool is_not);

    /**
     * @brief 解析函数调用表达式
     * @param line 行号
     * @param column 列号
     * @param function_name 函数名
     * @return 函数调用表达式节点
     */
    std::unique_ptr<ast::AstExpression> parse_function_call_expression(
        std::size_t line,
        std::size_t column,
        const std::string & function_name
    );

    /**
     * @brief 解析列引用表达式
     * @param line 行号
     * @param column 列号
     * @param first_part 第一个标识符部分
     * @return 列引用表达式节点
     */
    std::unique_ptr<ast::AstExpression> parse_column_reference_expression(
        std::size_t line,
        std::size_t column,
        const std::string & first_part
    );

    // ========== 语法分析辅助方法 ==========

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

} // namespace dreamdb::parser
