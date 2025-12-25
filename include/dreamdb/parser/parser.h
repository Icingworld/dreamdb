#pragma once

#include <cstdint>
#include <cstddef>
#include <memory>
#include <string>
#include <stdexcept>

#include "dreamdb/parser/lexer.h"
#include "dreamdb/parser/token.h"
#include "dreamdb/parser/ast/ast_node.h"
#include "dreamdb/parser/ast/create_stmt.h"

namespace dreamdb
{

// 前向声明所有 AST 节点
class SelectStmt;
class InsertStmt;
class UpdateStmt;
class DeleteStmt;
class CreateStmt;
class DropStmt;
class UseStmt;
class AlterStmt;
class ShowStmt;
class DescribeStmt;
class UnaryExpr;
class BinaryExpr;
class FunctionCallExpr;
class InExpr;
class LikeExpr;
class BetweenExpr;
class NullExpr;
class IdentifierExpr;
class LiteralExpr;
class ColumnDefinition;

// 前向声明类型
enum class FieldType : std::uint8_t;

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

    ~ParseException() noexcept = default;

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
    /**
     * @brief 构造函数
     * @param input SQL 字符串
     */
    explicit Parser(const std::string & input);

    /**
     * @brief 构造函数（从 Lexer 构建）
     * @param lexer 词法分析器
     */
    explicit Parser(std::unique_ptr<Lexer> lexer);

    Parser(const Parser &) = delete;

    Parser(Parser &&) noexcept = default;

    Parser & operator=(const Parser &) = delete;

    Parser & operator=(Parser &&) noexcept = default;

    ~Parser() = default;

public:
    /**
     * @brief 解析 SQL 语句
     * @return AST 根节点
     * @throws ParseException 如果解析失败
     */
    std::unique_ptr<AstNode> parse();

private:
    // ========== 语句解析 ==========

    /**
     * @brief 解析单个语句
     * @return 语句节点
     */
    std::unique_ptr<AstNode> parse_statement();

    /**
     * @brief 解析 SELECT 语句
     */
    std::unique_ptr<SelectStmt> parse_select_stmt();

    /**
     * @brief 解析 INSERT 语句
     */
    std::unique_ptr<InsertStmt> parse_insert_stmt();

    /**
     * @brief 解析 UPDATE 语句
     */
    std::unique_ptr<UpdateStmt> parse_update_stmt();

    /**
     * @brief 解析 DELETE 语句
     */
    std::unique_ptr<DeleteStmt> parse_delete_stmt();

    /**
     * @brief 解析 CREATE 语句
     */
    std::unique_ptr<CreateStmt> parse_create_stmt();

    /**
     * @brief 解析 DROP 语句
     */
    std::unique_ptr<DropStmt> parse_drop_stmt();

    /**
     * @brief 解析 USE 语句
     */
    std::unique_ptr<UseStmt> parse_use_stmt();

    /**
     * @brief 解析 ALTER 语句
     */
    std::unique_ptr<AlterStmt> parse_alter_stmt();

    /**
     * @brief 解析 SHOW 语句
     * SHOW TABLES/COLLECTIONS
     */
    std::unique_ptr<ShowStmt> parse_show_stmt();

    /**
     * @brief 解析 DESCRIBE 语句
     */
    std::unique_ptr<DescribeStmt> parse_describe_stmt();

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
    std::unique_ptr<AstNode> parse_expression();

    /**
     * @brief 解析逻辑 OR 表达式
     */
    std::unique_ptr<AstNode> parse_or_expression();

    /**
     * @brief 解析逻辑 AND 表达式
     */
    std::unique_ptr<AstNode> parse_and_expression();

    /**
     * @brief 解析比较表达式
     */
    std::unique_ptr<AstNode> parse_comparison_expression();

    /**
     * @brief 解析 LIKE 表达式
     */
    std::unique_ptr<LikeExpr> parse_like_expression(std::unique_ptr<AstNode> left);

    /**
     * @brief 解析 IN 表达式
     */
    std::unique_ptr<InExpr> parse_in_expression(std::unique_ptr<AstNode> left);

    /**
     * @brief 解析 BETWEEN 表达式
     */
    std::unique_ptr<BetweenExpr> parse_between_expression(std::unique_ptr<AstNode> left);

    /**
     * @brief 解析 NULL 表达式
     */
    std::unique_ptr<NullExpr> parse_null_expression(std::unique_ptr<AstNode> left);

    /**
     * @brief 解析算术加减表达式
     */
    std::unique_ptr<AstNode> parse_additive_expression();

    /**
     * @brief 解析算术乘除表达式
     */
    std::unique_ptr<AstNode> parse_multiplicative_expression();

    /**
     * @brief 解析一元表达式
     */
    std::unique_ptr<AstNode> parse_unary_expression();

    /**
     * @brief 解析基础表达式（标识符、字面量、函数调用、括号表达式）
     */
    std::unique_ptr<AstNode> parse_primary_expression();

    /**
     * @brief 解析函数调用
     * function_name ( [arguments] )
     */
    std::unique_ptr<FunctionCallExpr> parse_function_call();

    /**
     * @brief 解析标识符表达式
     * identifier [. identifier] ...
     */
    std::unique_ptr<IdentifierExpr> parse_identifier_expr();

    /**
     * @brief 解析字面量表达式
     */
    std::unique_ptr<LiteralExpr> parse_literal_expr();

    /**
     * @brief 解析字符串字面量
     */
    std::unique_ptr<LiteralExpr> parse_string_literal();

    /**
     * @brief 解析数字字面量
     */
    std::unique_ptr<LiteralExpr> parse_number_literal();

    /**
     * @brief 解析布尔字面量
     */
    std::unique_ptr<LiteralExpr> parse_boolean_literal();

    /**
     * @brief 解析 NULL 字面量
     */
    std::unique_ptr<LiteralExpr> parse_null_literal();

    /**
     * @brief 解析向量字面量
     */
    std::unique_ptr<LiteralExpr> parse_vector_literal();

    // ========== CREATE 语句辅助方法 ==========

    /**
     * @brief 解析列定义
     * column_name type [parameters] [attributes]
     */
    ColumnDefinition parse_column_definition();

    /**
     * @brief 解析向量索引 WITH 子句
     */
    void parse_vindex_with_clause(VIndexWithClause & with_clause);

    /**
     * @brief 解析字段类型
     * INT8 | INT16 | INT32 | INT64 | FLOAT | DOUBLE | CHAR | VARCHAR | BOOLEAN | TIMESTAMP | ENUM | FLOAT_VECTOR
     */
    FieldType parse_field_type();

    // ========== 辅助方法 ==========

    /**
     * @brief 获取当前 Token（不消耗）
     */
    const Token & current() const;

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
    bool has_error_;                    // 是否有错误
};

} // namespace dreamdb
