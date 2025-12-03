#include "dreamdb/parser/parser.h"

#include <algorithm>
#include <cctype>

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
    // 解析左侧的 AND 表达式
    auto left = parse_and_expression();
    
    // 循环处理多个 OR 运算符（左结合）
    // 例如: a OR b OR c 解析为 ((a OR b) OR c)
    while (check(TokenType::OR)) {
        // 保存 OR token 的位置信息
        std::size_t line = current_token.get_line();
        std::size_t column = current_token.get_column();
        
        // 消耗 OR token
        advance();
        
        // 创建二元表达式节点
        auto expr = std::make_unique<BinaryExpr>(line, column);
        
        // 设置运算符类型为 OR
        expr->set_op_type(BinaryOperatorType::OR);
        
        // 设置左操作数为之前解析的结果
        expr->set_left(std::move(left));
        
        // 解析右侧的 AND 表达式
        auto right = parse_and_expression();
        expr->set_right(std::move(right));
        
        // 将当前表达式作为下一次的左操作数
        left = std::move(expr);
    }
    
    return left;
}

std::unique_ptr<AstNode> Parser::parse_and_expression()
{
    // 解析左侧的比较表达式
    auto left = parse_comparison_expression();
    
    // 循环处理多个 AND 运算符（左结合）
    // 例如: a AND b AND c 解析为 ((a AND b) AND c)
    while (check(TokenType::AND)) {
        // 保存 AND token 的位置信息
        std::size_t line = current_token.get_line();
        std::size_t column = current_token.get_column();
        
        // 消耗 AND token
        advance();
        
        // 创建二元表达式节点
        auto expr = std::make_unique<BinaryExpr>(line, column);
        
        // 设置运算符类型为 AND
        expr->set_op_type(BinaryOperatorType::AND);
        
        // 设置左操作数为之前解析的结果
        expr->set_left(std::move(left));
        
        // 解析右侧的比较表达式
        auto right = parse_comparison_expression();
        expr->set_right(std::move(right));
        
        // 将当前表达式作为下一次的左操作数
        left = std::move(expr);
    }
    
    return left;
}

std::unique_ptr<AstNode> Parser::parse_comparison_expression()
{
    // 解析左侧的加减表达式
    auto left = parse_additive_expression();
    
    // 检查是否有比较运算符
    BinaryOperatorType op_type;
    bool has_comparison = false;
    std::size_t line = 0;
    std::size_t column = 0;
    
    // 根据当前 token 判断比较运算符类型
    switch (current_token.get_type()) {
        case TokenType::EQUAL:
            line = current_token.get_line();
            column = current_token.get_column();
            op_type = BinaryOperatorType::EQUAL;
            has_comparison = true;
            advance();
            break;
        case TokenType::NOT_EQUAL:
            line = current_token.get_line();
            column = current_token.get_column();
            op_type = BinaryOperatorType::NOT_EQUAL;
            has_comparison = true;
            advance();
            break;
        case TokenType::LESS_THAN:
            line = current_token.get_line();
            column = current_token.get_column();
            op_type = BinaryOperatorType::LESS_THAN;
            has_comparison = true;
            advance();
            break;
        case TokenType::GREATER_THAN:
            line = current_token.get_line();
            column = current_token.get_column();
            op_type = BinaryOperatorType::GREATER_THAN;
            has_comparison = true;
            advance();
            break;
        case TokenType::LESS_EQUAL:
            line = current_token.get_line();
            column = current_token.get_column();
            op_type = BinaryOperatorType::LESS_EQUAL;
            has_comparison = true;
            advance();
            break;
        case TokenType::GREATER_EQUAL:
            line = current_token.get_line();
            column = current_token.get_column();
            op_type = BinaryOperatorType::GREATER_EQUAL;
            has_comparison = true;
            advance();
            break;
        default:
            // 没有比较运算符，直接返回左侧表达式
            return left;
    }
    
    // 如果有比较运算符，创建二元表达式节点
    if (has_comparison) {
        auto expr = std::make_unique<BinaryExpr>(line, column);
        expr->set_op_type(op_type);
        expr->set_left(std::move(left));
        
        // 解析右侧的加减表达式
        auto right = parse_additive_expression();
        expr->set_right(std::move(right));
        
        return expr;
    }
    
    // 理论上不会执行到这里，但为了完整性保留
    return left;
}

std::unique_ptr<AstNode> Parser::parse_additive_expression()
{
    // 解析左侧的乘除表达式
    auto left = parse_multiplicative_expression();

    // 循环处理加减运算符（左结合）
    while (check(TokenType::PLUS) || check(TokenType::MINUS)) {
        // 保存运算符信息
        TokenType op_token_type = current_token.get_type();
        std::size_t line = current_token.get_line();
        std::size_t column = current_token.get_column();

        // 消耗运算符
        advance();

        // 创建二元表达式节点
        auto expr = std::make_unique<BinaryExpr>(line, column);

        // 设置运算符类型
        if (op_token_type == TokenType::PLUS) {
            expr->set_op_type(BinaryOperatorType::PLUS);
        } else {
            expr->set_op_type(BinaryOperatorType::MINUS);
        }

        // 设置左操作数为之前解析的结果
        expr->set_left(std::move(left));

        // 解析右侧的乘除表达式
        auto right = parse_multiplicative_expression();
        expr->set_right(std::move(right));

        // 将当前表达式作为下一次的左操作数
        left = std::move(expr);
    }

    return left;
}

std::unique_ptr<AstNode> Parser::parse_multiplicative_expression()
{
    // 解析左侧的一元表达式
    auto left = parse_unary_expression();

    // 循环处理乘除模运算符（左结合）
    while (check(TokenType::MULTIPLY) ||
           check(TokenType::DIVIDE) ||
           check(TokenType::MODULO)) {
        // 保存运算符信息
        TokenType op_token_type = current_token.get_type();
        std::size_t line = current_token.get_line();
        std::size_t column = current_token.get_column();

        // 消耗运算符
        advance();

        // 创建二元表达式节点
        auto expr = std::make_unique<BinaryExpr>(line, column);

        // 设置运算符类型
        switch (op_token_type) {
            case TokenType::MULTIPLY:
                expr->set_op_type(BinaryOperatorType::MULTIPLY);
                break;
            case TokenType::DIVIDE:
                expr->set_op_type(BinaryOperatorType::DIVIDE);
                break;
            case TokenType::MODULO:
                expr->set_op_type(BinaryOperatorType::MODULO);
                break;
            default:
                // 理论上不会到这里
                error("Unexpected token in multiplicative expression: " + current_token.to_string());
        }

        // 设置左操作数为之前解析的结果
        expr->set_left(std::move(left));

        // 解析右侧的一元表达式
        auto right = parse_unary_expression();
        expr->set_right(std::move(right));

        // 将当前表达式作为下一次的左操作数
        left = std::move(expr);
    }

    return left;
}

std::unique_ptr<AstNode> Parser::parse_unary_expression()
{
    // 一元运算符：NOT, +, -
    if (check(TokenType::NOT) ||
        check(TokenType::PLUS) ||
        check(TokenType::MINUS)) {
        TokenType op_token_type = current_token.get_type();
        std::size_t line = current_token.get_line();
        std::size_t column = current_token.get_column();

        // 消耗一元运算符
        advance();

        auto expr = std::make_unique<UnaryExpr>(line, column);

        // 设置运算符类型
        switch (op_token_type) {
            case TokenType::NOT:
                expr->set_op_type(UnaryOperatorType::NOT);
                break;
            case TokenType::PLUS:
                expr->set_op_type(UnaryOperatorType::PLUS);
                break;
            case TokenType::MINUS:
                expr->set_op_type(UnaryOperatorType::MINUS);
                break;
            default:
                // 理论上不会到这里
                error("Unexpected token in unary expression: " + current_token.to_string());
        }

        // 递归解析操作数（仍然是 unary_expression，保证右结合）
        auto operand = parse_unary_expression();
        expr->set_operand(std::move(operand));

        return expr;
    }

    // 否则解析基础表达式
    return parse_primary_expression();
}

std::unique_ptr<AstNode> Parser::parse_primary_expression()
{
    // 括号表达式
    if (check(TokenType::LEFT_PAREN)) {
        // 消耗 '('
        advance();

        // 解析括号中的表达式
        auto expr = parse_expression();

        // 期望并消耗 ')'
        consume(TokenType::RIGHT_PAREN, "Expected ')' after expression");
        return expr;
    }

    // 标识符：可能是函数调用或普通标识符
    if (check(TokenType::IDENTIFIER)) {
        // 通过 Lexer 的 peek_token 判断是否为函数调用
        Token next = lexer->peek_token();
        if (next.get_type() == TokenType::LEFT_PAREN) {
            return parse_function_call();
        } else {
            return parse_identifier_expr();
        }
    }

    // 字面量：数字、字符串、布尔值、NULL
    if (check(TokenType::NUMBER_LITERAL) ||
        check(TokenType::STRING_LITERAL) ||
        check(TokenType::BOOLEAN_LITERAL) ||
        check(TokenType::NULL_LITERAL)) {
        return parse_literal_expr();
    }

    // 其他情况都是错误
    error("Expected primary expression, but got: " + current_token.to_string());
    return nullptr;
}

std::unique_ptr<FunctionCallExpr> Parser::parse_function_call()
{
    // 当前 token 应该是函数名（标识符）
    if (!check(TokenType::IDENTIFIER)) {
        error("Expected function name before '(' in function call, but got: " + current_token.to_string());
    }

    std::string function_name = current_token.get_value();
    std::size_t line = current_token.get_line();
    std::size_t column = current_token.get_column();

    // 消耗函数名
    advance();

    // 期望 '('
    consume(TokenType::LEFT_PAREN, "Expected '(' after function name");

    auto func = std::make_unique<FunctionCallExpr>(line, column);
    func->set_function_name(function_name);

    // 处理无参数情况：立即遇到 ')'
    if (check(TokenType::RIGHT_PAREN)) {
        advance(); // 消耗 ')'
        return func;
    }

    // 解析第一个参数
    func->add_argument(parse_expression());

    // 解析后续参数
    while (match(TokenType::COMMA)) {
        func->add_argument(parse_expression());
    }

    // 期望 ')'
    consume(TokenType::RIGHT_PAREN, "Expected ')' after function arguments");

    return func;
}

std::unique_ptr<IdentifierExpr> Parser::parse_identifier_expr()
{
    if (!check(TokenType::IDENTIFIER)) {
        error("Expected identifier, but got: " + current_token.to_string());
    }

    std::size_t line = current_token.get_line();
    std::size_t column = current_token.get_column();
    std::string first_part = current_token.get_value();

    auto ident = std::make_unique<IdentifierExpr>(line, column);
    ident->set_type(IdentifierType::COLUMN); // 默认视为列名，后续可根据上下文调整
    ident->add_part(first_part);
    ident->set_original_text(first_part);

    // 消耗第一个标识符
    advance();

    // 处理限定名：schema.table.column
    while (match(TokenType::DOT)) {
        if (!check(TokenType::IDENTIFIER)) {
            error("Expected identifier after '.', but got: " + current_token.to_string());
        }
        std::string part = current_token.get_value();
        ident->add_part(part);
        // 更新 original_text 表示完整限定名
        ident->set_original_text(ident->get_original_text() + "." + part);

        advance();
    }

    return ident;
}

std::unique_ptr<LiteralExpr> Parser::parse_literal_expr()
{
    switch (current_token.get_type()) {
        case TokenType::STRING_LITERAL:
            return parse_string_literal();
        case TokenType::NUMBER_LITERAL:
            return parse_number_literal();
        case TokenType::BOOLEAN_LITERAL:
            return parse_boolean_literal();
        case TokenType::NULL_LITERAL:
            return parse_null_literal();
        default:
            error("Expected literal expression, but got: " + current_token.to_string());
            return nullptr;
    }
}

std::unique_ptr<LiteralExpr> Parser::parse_string_literal()
{
    if (!check(TokenType::STRING_LITERAL)) {
        error("Expected string literal, but got: " + current_token.to_string());
    }

    std::size_t line = current_token.get_line();
    std::size_t column = current_token.get_column();
    std::string value = current_token.get_value();

    auto literal = std::make_unique<LiteralExpr>(line, column);
    literal->set_literal_type(LiteralType::STRING);
    literal->set_value(LiteralValue{value});

    advance(); // 消耗字面量 token
    return literal;
}

std::unique_ptr<LiteralExpr> Parser::parse_number_literal()
{
    if (!check(TokenType::NUMBER_LITERAL)) {
        error("Expected number literal, but got: " + current_token.to_string());
    }

    std::size_t line = current_token.get_line();
    std::size_t column = current_token.get_column();
    const std::string & text = current_token.get_value();

    auto literal = std::make_unique<LiteralExpr>(line, column);

    try {
        // 判断是否为浮点数：包含 '.' 或 'e'/'E'
        bool is_float = (text.find('.') != std::string::npos) ||
                        (text.find('e') != std::string::npos) ||
                        (text.find('E') != std::string::npos);

        if (is_float) {
            double v = std::stod(text);
            literal->set_literal_type(LiteralType::FLOAT);
            literal->set_value(LiteralValue{v});
        } else {
            long long v = std::stoll(text);
            literal->set_literal_type(LiteralType::INTERGER);
            literal->set_value(LiteralValue{static_cast<std::int64_t>(v)});
        }
    } catch (const std::exception & e) {
        error(std::string("Invalid number literal '") + text + "': " + e.what());
    }

    advance(); // 消耗字面量 token
    return literal;
}

std::unique_ptr<LiteralExpr> Parser::parse_boolean_literal()
{
    if (!check(TokenType::BOOLEAN_LITERAL)) {
        error("Expected boolean literal, but got: " + current_token.to_string());
    }

    std::size_t line = current_token.get_line();
    std::size_t column = current_token.get_column();
    std::string text = current_token.get_value();

    // 转为大写进行比较
    std::transform(text.begin(), text.end(), text.begin(),
                   [](unsigned char c) { return static_cast<char>(std::toupper(c)); });

    bool value;
    if (text == "TRUE") {
        value = true;
    } else if (text == "FALSE") {
        value = false;
    } else {
        error("Invalid boolean literal: " + current_token.to_string());
        return nullptr;
    }

    auto literal = std::make_unique<LiteralExpr>(line, column);
    literal->set_literal_type(LiteralType::BOOLEAN);
    literal->set_value(LiteralValue{value});

    advance(); // 消耗字面量 token
    return literal;
}

std::unique_ptr<LiteralExpr> Parser::parse_null_literal()
{
    if (!check(TokenType::NULL_LITERAL)) {
        error("Expected NULL literal, but got: " + current_token.to_string());
    }

    std::size_t line = current_token.get_line();
    std::size_t column = current_token.get_column();

    auto literal = std::make_unique<LiteralExpr>(line, column);
    literal->set_literal_type(LiteralType::NULL_VALUE);
    literal->set_value(LiteralValue{NullType()});

    advance(); // 消耗 NULL token
    return literal;
}

} // namespace dreamdb
