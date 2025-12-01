#include "dreamdb/parser/parser.h"

#include "dreamdb/parser/ast/select_stmt.h"
#include "dreamdb/parser/ast/insert_stmt.h"
#include "dreamdb/parser/ast/update_stmt.h"
#include "dreamdb/parser/ast/delete_stmt.h"
#include "dreamdb/parser/ast/create_stmt.h"
#include "dreamdb/parser/ast/drop_stmt.h"
#include "dreamdb/parser/ast/unary_expr.h"
#include "dreamdb/parser/ast/binary_expr.h"
#include "dreamdb/parser/ast/function_call_expr.h"
#include "dreamdb/parser/ast/identifier_expr.h"
#include "dreamdb/parser/ast/literal_expr.h"

namespace dreamdb
{

ParseException::ParseException(const std::string & message, std::size_t line, std::size_t column)
    : std::runtime_error(message)
    , line(line)
    , column(column)
{
}

std::size_t ParseException::get_line() const noexcept
{
    return line;
}

std::size_t ParseException::get_column() const noexcept
{
    return column;
}

std::string ParseException::get_message() const noexcept
{
    return std::string(what()) + " at line " + std::to_string(line) + ", column " + std::to_string(column);
}

Parser::Parser(const std::string & input)
    : lexer(std::make_unique<Lexer>(input))
    , current_token(TokenType::EOF_TOKEN)
    , has_error(false)
{
}

Parser::Parser(std::unique_ptr<Lexer> lexer)
    : lexer(std::move(lexer))
    , current_token(TokenType::EOF_TOKEN)
    , has_error(false)
{
}

std::unique_ptr<AstNode> Parser::parse()
{
    // 初始化：读取第一个 Token
    current_token = lexer->next_token();
    has_error = false;

    // 检查是否为空输入
    if (current_token.get_type() == TokenType::EOF_TOKEN) {
        error("Unexpected end of input");
    }

    // 解析单个语句
    auto stmt = parse_statement();

    // 跳过分号（如果存在）
    skip_semicolon();

    // 检查是否还有更多内容
    if (current_token.get_type() != TokenType::EOF_TOKEN) {
        // 不应该有更多内容，有则非法
        error("Unexpected token after statement: " + current_token.to_string());
    }

    // 如果解析过程中发生了错误，stmt 应该已经被设置为 nullptr 或者抛出了异常
    // has_error 标志主要用于内部状态跟踪，error() 函数会直接抛出异常
    return stmt;
}

std::unique_ptr<AstNode> Parser::parse_statement()
{
    // 根据当前 Token 的类型判断是哪种语句
    switch (current_token.get_type()) {
        case TokenType::SELECT:
            return parse_select_stmt();
        case TokenType::INSERT:
            return parse_insert_stmt();
        case TokenType::UPDATE:
            return parse_update_stmt();
        case TokenType::DELETE:
            return parse_delete_stmt();
        case TokenType::CREATE:
            return parse_create_stmt();
        case TokenType::DROP:
            return parse_drop_stmt();
        default:
            error("Expected a statement (SELECT, INSERT, UPDATE, DELETE, CREATE, or DROP), but got: " + current_token.to_string());
            return nullptr;
    }
}

const Token & Parser::current() const
{
    return current_token;
}

Token Parser::advance()
{
    Token prev = current_token;
    if (lexer->has_more()) {
        current_token = lexer->next_token();
    } else {
        current_token = Token(
            TokenType::EOF_TOKEN, 
            "", 
            current_token.get_line(), 
            current_token.get_column() + 1);
    }
    return prev;
}

bool Parser::match(TokenType type)
{
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(TokenType type) const
{
    return current_token.get_type() == type;
}

void Parser::consume(TokenType type, const std::string & message)
{
    if (check(type)) {
        advance();
    } else {
        error(message + ", but got: " + current_token.to_string());
    }
}

bool Parser::is_expression_terminator() const
{
    TokenType type = current_token.get_type();
    return type == TokenType::EOF_TOKEN
        || type == TokenType::SEMICOLON
        || type == TokenType::COMMA
        || type == TokenType::RIGHT_PAREN
        || type == TokenType::WHERE
        || type == TokenType::FROM;
}

void Parser::skip_semicolon()
{
    match(TokenType::SEMICOLON);
}

[[noreturn]] void Parser::error(const std::string & message)
{
    has_error = true;
    throw ParseException(message, current_token.get_line(), current_token.get_column());
}

std::unique_ptr<SelectStmt> Parser::parse_select_stmt()
{
    // TODO: 实现 SELECT 语句解析
    error("parse_select_stmt() not yet implemented");
}

std::unique_ptr<InsertStmt> Parser::parse_insert_stmt()
{
    // TODO: 实现 INSERT 语句解析
    error("parse_insert_stmt() not yet implemented");
}

std::unique_ptr<UpdateStmt> Parser::parse_update_stmt()
{
    // TODO: 实现 UPDATE 语句解析
    error("parse_update_stmt() not yet implemented");
}

std::unique_ptr<DeleteStmt> Parser::parse_delete_stmt()
{
    // DELETE 语句示例
    // DELETE FROM my_collection;
    // DELETE FROM my_collection WHERE my_column = 'value';

    // 获取 DELETE 关键字的位置信息
    std::size_t line = current_token.get_line();
    std::size_t column = current_token.get_column();

    // 创建 DeleteStmt 节点
    auto stmt = std::make_unique<DeleteStmt>(line, column);

    // 消耗 DELETE 关键字
    advance();

    // 解析 FROM 关键字
    if (match(TokenType::FROM)) {
        // 解析集合名称
        if (current_token.get_type() != TokenType::IDENTIFIER) {
            error("Expected identifier (collection name) after FROM, but got: " + current_token.to_string());
        }
        std::string collection_name = current_token.get_value();
        advance();
        stmt->set_collection_name(collection_name);
    }

    // 如果有 WHERE 关键字，则解析 WHERE 条件
    if (match(TokenType::WHERE)) {
        // 解析 WHERE 条件
        auto where_clause = parse_expression();
        stmt->set_where_clause(std::move(where_clause));
    }

    // DELETE 语句解析完成
    return stmt;
}

std::unique_ptr<CreateStmt> Parser::parse_create_stmt()
{
    // TODO: 实现 CREATE 语句解析
    error("parse_create_stmt() not yet implemented");
}

std::unique_ptr<DropStmt> Parser::parse_drop_stmt()
{
    // DROP 语句示例
    // DROP COLLECTION my_collection;

    // 获取 DROP 关键字的位置信息
    std::size_t line = current_token.get_line();
    std::size_t column = current_token.get_column();

    // 创建 DropStmt 节点
    auto stmt = std::make_unique<DropStmt>(line, column);

    // 消耗 DROP 关键字
    advance();

    // 解析对象类型：COLLECTION 或 INDEX
    DropStmt::ObjectType object_type;
    if (match(TokenType::COLLECTION)) {
        object_type = DropStmt::ObjectType::COLLECTION;
    } else if (match(TokenType::INDEX)) {
        object_type = DropStmt::ObjectType::INDEX;
        // TODO: 实现 INDEX 解析
        error("parse_drop_stmt(index) not yet implemented");
    } else {
        error("Expected COLLECTION after DROP, but got: " + current_token.to_string());
    }

    stmt->set_object_type(object_type);

    // 解析对象名称（标识符）
    if (current_token.get_type() != TokenType::IDENTIFIER) {
        error("Expected identifier (object name) after DROP COLLECTION, but got: " + current_token.to_string());
    }

    std::string object_name = current_token.get_value();
    // 消耗标识符 token
    advance();

    stmt->set_object_name(object_name);

    // DROP 语句解析完成
    return stmt;
}

std::unique_ptr<AstNode> Parser::parse_expression()
{
    // 该接口只是表达式解析入口，具体解析逻辑在各个解析函数中实现
    // 递归下降解析器会自动处理所有优先级

    // 表达式优先级：
    // 1. OR          (最低优先级，如: a OR b)
    // 2. AND         (如: a AND b)
    // 3. 比较运算符   (如: =, !=, <, >, <=, >=)
    // 4. 加减        (如: +, -)
    // 5. 乘除        (如: *, /, %)
    // 6. 一元运算符   (如: +, -, NOT)
    // 7. 基础表达式   (最高优先级，如: 字面量、标识符、括号、函数调用)

    // 完整解析流程：
    // parse_expression()
    //   └─> parse_or_expression()
    //    ├─> 解析左侧: parse_and_expression()
    //    │     ├─> 解析左侧: parse_comparison_expression()
    //    │     │     ├─> 解析左侧: parse_additive_expression()
    //    │     │     │     ├─> 解析左侧: parse_multiplicative_expression()
    //    │     │     │     │     ├─> 解析左侧: parse_unary_expression()
    //    │     │     │     │     │     └─> parse_primary_expression()  ← 基础表达式
    //    │     │     │     │     └─> 解析右侧: parse_unary_expression()
    //    │     │     │     └─> 解析右侧: parse_multiplicative_expression()
    //    │     │     └─> 解析右侧: parse_additive_expression()
    //    │     └─> 解析右侧: parse_comparison_expression()
    //    └─> 解析右侧: parse_and_expression()

    // 根据当前 Token 的类型判断是哪种表达式
    switch (current_token.get_type()) {
        // 字面量：数字、字符串、布尔值
        case TokenType::NUMBER_LITERAL:
        case TokenType::STRING_LITERAL:
        case TokenType::BOOLEAN_LITERAL:
        // 标识符：字段名、变量等
        case TokenType::IDENTIFIER:
        // 一元运算符：+、-、NOT
        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::NOT:
        // 括号表达式
        case TokenType::LEFT_PAREN:
            // 调用最低优先级解析函数
            return parse_or_expression();
        default:
            // 非法的表达式开始 token
            error("Expected expression, but got: " + current_token.to_string());
            return nullptr;
    }
}

std::unique_ptr<AstNode> Parser::parse_or_expression()
{
    // TODO: 实现 OR 表达式解析
    error("parse_or_expression() not yet implemented");
}

std::unique_ptr<AstNode> Parser::parse_and_expression()
{
    // TODO: 实现 AND 表达式解析
    error("parse_and_expression() not yet implemented");
}

std::unique_ptr<AstNode> Parser::parse_comparison_expression()
{
    // TODO: 实现比较表达式解析
    error("parse_comparison_expression() not yet implemented");
}

std::unique_ptr<AstNode> Parser::parse_additive_expression()
{
    // TODO: 实现加减表达式解析
    error("parse_additive_expression() not yet implemented");
}

std::unique_ptr<AstNode> Parser::parse_multiplicative_expression()
{
    // TODO: 实现乘除表达式解析
    error("parse_multiplicative_expression() not yet implemented");
}

std::unique_ptr<AstNode> Parser::parse_unary_expression()
{
    // TODO: 实现一元表达式解析
    error("parse_unary_expression() not yet implemented");
}

std::unique_ptr<AstNode> Parser::parse_primary_expression()
{
    // TODO: 实现基础表达式解析
    error("parse_primary_expression() not yet implemented");
}

std::unique_ptr<FunctionCallExpr> Parser::parse_function_call()
{
    // TODO: 实现函数调用解析
    error("parse_function_call() not yet implemented");
}

std::unique_ptr<IdentifierExpr> Parser::parse_identifier_expr()
{
    // TODO: 实现标识符表达式解析
    error("parse_identifier_expr() not yet implemented");
}

std::unique_ptr<LiteralExpr> Parser::parse_literal_expr()
{
    // TODO: 实现字面量表达式解析
    error("parse_literal_expr() not yet implemented");
}

std::unique_ptr<LiteralExpr> Parser::parse_string_literal()
{
    // TODO: 实现字符串字面量解析
    error("parse_string_literal() not yet implemented");
}

std::unique_ptr<LiteralExpr> Parser::parse_number_literal()
{
    // TODO: 实现数字字面量解析
    error("parse_number_literal() not yet implemented");
}

std::unique_ptr<LiteralExpr> Parser::parse_boolean_literal()
{
    // TODO: 实现布尔字面量解析
    error("parse_boolean_literal() not yet implemented");
}

std::unique_ptr<LiteralExpr> Parser::parse_null_literal()
{
    // TODO: 实现 NULL 字面量解析
    error("parse_null_literal() not yet implemented");
}

} // namespace dreamdb
