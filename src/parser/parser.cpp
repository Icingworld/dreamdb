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

    // 检查是否还有更多内容（不应该有）
    if (current_token.get_type() != TokenType::EOF_TOKEN) {
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
    // TODO: 实现 DELETE 语句解析
    error("parse_delete_stmt() not yet implemented");
}

std::unique_ptr<CreateStmt> Parser::parse_create_stmt()
{
    // TODO: 实现 CREATE 语句解析
    error("parse_create_stmt() not yet implemented");
}

std::unique_ptr<DropStmt> Parser::parse_drop_stmt()
{
    // 获取 DROP 关键字的位置信息
    // 注意：parse_statement() 中的 switch 没有消耗 token，所以当前 token 仍然是 DROP
    std::size_t line = current_token.get_line();
    std::size_t column = current_token.get_column();

    // 创建 DropStmt 节点
    auto stmt = std::make_unique<DropStmt>(line, column);

    // 消耗 DROP 关键字
    advance();

    // 解析对象类型：COLLECTION 或 INDEX(TODO)
    DropStmt::ObjectType object_type;
    if (match(TokenType::COLLECTION)) {
        object_type = DropStmt::ObjectType::COLLECTION;
    } else {
        error("Expected COLLECTION after DROP, but got: " + current_token.to_string());
    }

    stmt->set_object_type(object_type);

    // 解析对象名称（标识符）
    if (current_token.get_type() != TokenType::IDENTIFIER) {
        error("Expected identifier (object name) after DROP COLLECTION, but got: " + current_token.to_string());
    }

    std::string object_name = current_token.get_value();
    advance(); // 消耗标识符 token

    stmt->set_object_name(object_name);

    return stmt;
}

std::unique_ptr<AstNode> Parser::parse_expression()
{
    // TODO: 实现表达式解析
    error("parse_expression() not yet implemented");
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
