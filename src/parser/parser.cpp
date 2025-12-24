#include "dreamdb/parser/parser.h"

#include <algorithm>
#include <cctype>
#include <limits>

#include "dreamdb/parser/ast/select_stmt.h"
#include "dreamdb/parser/ast/insert_stmt.h"
#include "dreamdb/parser/ast/update_stmt.h"
#include "dreamdb/parser/ast/delete_stmt.h"
#include "dreamdb/parser/ast/create_stmt.h"
#include "dreamdb/parser/ast/drop_stmt.h"
#include "dreamdb/parser/ast/use_stmt.h"
#include "dreamdb/parser/ast/alter_stmt.h"
#include "dreamdb/parser/ast/show_stmt.h"
#include "dreamdb/parser/ast/describe_stmt.h"
#include "dreamdb/parser/ast/unary_expr.h"
#include "dreamdb/parser/ast/binary_expr.h"
#include "dreamdb/parser/ast/function_call_expr.h"
#include "dreamdb/parser/ast/identifier_expr.h"
#include "dreamdb/parser/ast/literal_expr.h"

namespace dreamdb
{

ParseException::ParseException(const std::string & message, std::size_t line, std::size_t column)
    : std::runtime_error(message)
    , line_(line)
    , column_(column)
{
}

std::size_t ParseException::get_line() const noexcept
{
    return line_;
}

std::size_t ParseException::get_column() const noexcept
{
    return column_;
}

std::string ParseException::get_message() const noexcept
{
    return std::string(what()) + " at line " + std::to_string(line_) + ", column " + std::to_string(column_);
}

Parser::Parser(const std::string & input)
    : lexer_(std::make_unique<Lexer>(input))
    , current_token_(TokenType::DB_EOF_TOKEN)
    , has_error_(false)
{
}

Parser::Parser(std::unique_ptr<Lexer> lexer)
    : lexer_(std::move(lexer))
    , current_token_(TokenType::DB_EOF_TOKEN)
    , has_error_(false)
{
}

std::unique_ptr<AstNode> Parser::parse()
{
    // 初始化：读取第一个 Token
    current_token_ = lexer_->next_token();
    has_error_ = false;

    // 检查是否为空输入
    if (current_token_.get_type() == TokenType::DB_EOF_TOKEN) {
        error("Unexpected end of input");
    }

    // 解析单个语句
    auto stmt = parse_statement();

    // 跳过分号（如果存在）
    skip_semicolon();

    // 检查是否还有更多内容
    if (current_token_.get_type() != TokenType::DB_EOF_TOKEN) {
        // 不应该有更多内容，有则非法
        error("Unexpected token after statement: " + current_token_.to_string());
    }

    // 如果解析过程中发生了错误，stmt 应该已经被设置为 nullptr 或者抛出了异常
    // has_error_ 标志主要用于内部状态跟踪，error() 函数会直接抛出异常
    return stmt;
}

std::unique_ptr<AstNode> Parser::parse_statement()
{
    // 根据当前 Token 的类型判断是哪种语句
    switch (current_token_.get_type()) {
        case TokenType::DB_SELECT:
            return parse_select_stmt();
        case TokenType::DB_INSERT:
            return parse_insert_stmt();
        case TokenType::DB_UPDATE:
            return parse_update_stmt();
        case TokenType::DB_DELETE:
            return parse_delete_stmt();
        case TokenType::DB_CREATE:
            return parse_create_stmt();
        case TokenType::DB_DROP:
            return parse_drop_stmt();
        case TokenType::DB_USE:
            return parse_use_stmt();
        case TokenType::DB_ALTER:
            return parse_alter_stmt();
        case TokenType::DB_SHOW:
            return parse_show_stmt();
        case TokenType::DB_DESCRIBE:
        case TokenType::DB_DESC:
            return parse_describe_stmt();
        default:
            error("Expected SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, USE, ALTER, SHOW, or DESCRIBE, but got: " + current_token_.to_string());
    }
}

const Token & Parser::current() const
{
    return current_token_;
}

Token Parser::advance()
{
    Token prev = current_token_;
    if (lexer_->has_more()) {
        current_token_ = lexer_->next_token();
    } else {
        current_token_ = Token(
            TokenType::DB_EOF_TOKEN, 
            "", 
            current_token_.get_line(), 
            current_token_.get_column() + 1);
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
    return current_token_.get_type() == type;
}

void Parser::consume(TokenType type, const std::string & message)
{
    if (check(type)) {
        advance();
    } else {
        error(message + ", but got: " + current_token_.to_string());
    }
}

bool Parser::is_expression_terminator() const
{
    TokenType type = current_token_.get_type();
    return type == TokenType::DB_EOF_TOKEN
        || type == TokenType::DB_SEMICOLON
        || type == TokenType::DB_COMMA
        || type == TokenType::DB_RIGHT_PAREN
        || type == TokenType::DB_WHERE
        || type == TokenType::DB_FROM;
}

void Parser::skip_semicolon()
{
    match(TokenType::DB_SEMICOLON);
}

[[noreturn]] void Parser::error(const std::string & message)
{
    has_error_ = true;
    throw ParseException(message, current_token_.get_line(), current_token_.get_column());
}

std::unique_ptr<SelectStmt> Parser::parse_select_stmt()
{
    // SELECT 语句示例
    // SELECT * FROM my_collection;
    // SELECT my_column FROM my_collection;
    // SELECT my_column1, my_column2 FROM my_collection;
    // SELECT * FROM my_collection WHERE my_column = 'value';
    // SELECT * FROM my_collection WHERE my_column = 'value' AND my_column2 = 'value2';
    
    // 获取 SELECT 关键字的位置信息
    std::size_t line = current_token_.get_line();
    std::size_t column = current_token_.get_column();

    // 创建 SelectStmt 节点
    auto stmt = std::make_unique<SelectStmt>(line, column);

    // 消耗 SELECT 关键字
    advance();

    // 解析 SELECT 列表
    // 支持两种形式：
    // SELECT * FROM ...
    // SELECT column1, column2, ... FROM ...
    // SELECT COUNT(*) FROM ...
    // SELECT column1 AS alias FROM ...

    if (check(TokenType::DB_MULTIPLY)) {
        // 处理 SELECT *
        stmt->add_select_item(SelectItem::create_star_item());
        advance(); // 消耗 '*'
    } else {
        // 处理 SELECT column1, column2, ...
        // 至少需要一个表达式
        do {
            // 解析表达式（可以是标识符、函数调用等）
            auto expr = parse_expression();
            std::string alias = "";

            // 检查是否有别名（仅支持 AS alias 形式）
            if (match(TokenType::DB_AS)) {
                // AS 关键字后必须有标识符作为别名
                if (!check(TokenType::DB_IDENTIFIER)) {
                    error("Expected alias name after AS, but got: " + current_token_.to_string());
                }
                alias = current_token_.get_value();
                advance(); // 消耗别名标识符
            }

            stmt->add_select_item(SelectItem::create_expression_item(std::move(expr), alias));

            // 如果遇到逗号，继续解析下一个表达式
        } while (match(TokenType::DB_COMMA));
    }

    // 解析 FROM 关键字
    consume(TokenType::DB_FROM, "Expected FROM after SELECT list");

    // 解析表名
    if (!check(TokenType::DB_IDENTIFIER)) {
        error("Expected table name after FROM, but got: " + current_token_.to_string());
    }
    std::string collection_name = current_token_.get_value();
    stmt->set_collection_name(collection_name);
    advance(); // 消耗表名

    // 解析可选的 WHERE 子句
    if (match(TokenType::DB_WHERE)) {
        auto where_expr = parse_expression();
        stmt->set_where_clause(std::move(where_expr));
    }

    // 解析可选的 LIMIT 子句
    if (match(TokenType::DB_LIMIT)) {
        // LIMIT 后必须是数字字面量
        if (!check(TokenType::DB_NUMBER_LITERAL)) {
            error("Expected number after LIMIT, but got: " + current_token_.to_string());
        }
        
        // 解析 LIMIT 数字
        const std::string & limit_text = current_token_.get_value();
        try {
            // LIMIT 必须是正整数
            long long limit_value = std::stoll(limit_text);
            if (limit_value < 0) {
                error("LIMIT value must be non-negative, but got: " + limit_text);
            }
            std::size_t max_limit = std::numeric_limits<std::size_t>::max();
            if (limit_value > static_cast<long long>(max_limit)) {
                error("LIMIT value too large: " + limit_text);
            }
            stmt->set_limit(static_cast<std::size_t>(limit_value));
        } catch (const std::exception & e) {
            error("Invalid LIMIT value '" + limit_text + "': " + e.what());
        }
        
        advance(); // 消耗数字字面量
    }

    return stmt;
}

std::unique_ptr<InsertStmt> Parser::parse_insert_stmt()
{    
    // 获取 INSERT 关键字的位置信息
    std::size_t line = current_token_.get_line();
    std::size_t column = current_token_.get_column();
    
    // 创建 InsertStmt 节点
    auto stmt = std::make_unique<InsertStmt>(line, column);

    // 消耗 INSERT 关键字
    advance();
    
    // 解析 INTO 关键字
    consume(TokenType::DB_INTO, "Expected INTO after INSERT");

    // 解析集合名
    if (!check(TokenType::DB_IDENTIFIER)) {
        error("Expected collection_name after INTO");
    }
    std::string collection_name = current_token_.get_value();
    stmt->set_collection_name(collection_name);
    // 消耗集合名
    advance();

    // 解析可选的列名列表
    if (check(TokenType::DB_LEFT_PAREN)) {
        // 存在列名列表
        // 消耗 '('
        advance();

        // 解析列名列表
        do {
            if (!check(TokenType::DB_IDENTIFIER)) {
                error("Expected column_name, but got: " + current_token_.to_string());
            }
            std::string column_name = current_token_.get_value();
            stmt->add_column_name(column_name);
            // 消耗列名
            advance();
        } while (match(TokenType::DB_COMMA));

        // 期望 ')'
        consume(TokenType::DB_RIGHT_PAREN, "Expected ')' after column list");
    }

    // 解析 VALUES 关键字
    consume(TokenType::DB_VALUES, "Expected VALUES after collection_name or column list");

    // 解析值列表
    // 期望 '('
    consume(TokenType::DB_LEFT_PAREN, "Expected '(' after VALUES");

    // 解析值列表
    do {
        auto value_expr = parse_expression();
        stmt->add_value(std::move(value_expr));
    } while (match(TokenType::DB_COMMA));

    // 期望 ')'
    consume(TokenType::DB_RIGHT_PAREN, "Expected ')' after value list");

    // INSERT 语句解析完成
    return stmt;
}

std::unique_ptr<UpdateStmt> Parser::parse_update_stmt()
{
    // 获取 UPDATE 关键字的位置信息
    std::size_t line = current_token_.get_line();
    std::size_t column = current_token_.get_column();

    // 创建 UpdateStmt 节点
    auto stmt = std::make_unique<UpdateStmt>(line, column);

    // 消耗 UPDATE 关键字
    advance();

    // 解析集合名
    if (!check(TokenType::DB_IDENTIFIER)) {
        error("Expected collection_name after UPDATE");
    }
    std::string collection_name = current_token_.get_value();
    stmt->set_collection_name(collection_name);
    // 消耗集合名
    advance();

    // 解析 SET 关键字
    consume(TokenType::DB_SET, "Expected SET after collection_name");

    // 解析 SET 子句
    do {
        // 解析列名
        if (!check(TokenType::DB_IDENTIFIER)) {
            error("Expected column_name in SET clause, but got: " + current_token_.to_string());
        }
        std::string column_name = current_token_.get_value();
        // 消耗列名
        advance();

        // 解析 '='
        consume(TokenType::DB_EQUAL, "Expected '=' after column_name in SET clause");

        // 解析值表达式
        auto value_expr = parse_expression();
        stmt->add_assignment(column_name, std::move(value_expr));
    } while (match(TokenType::DB_COMMA));

    // 解析可选的 WHERE 子句
    if (match(TokenType::DB_WHERE)) {
        auto where_expr = parse_expression();
        stmt->set_where_clause(std::move(where_expr));
    }

    // 解析可选的 ORDER BY 子句
    if (match(TokenType::DB_ORDER)) {
        // 期望 BY 关键字
        consume(TokenType::DB_BY, "Expected BY after ORDER");

        // 解析列名
        if (!check(TokenType::DB_IDENTIFIER)) {
            error("Expected column name after ORDER BY");
        }
        std::string column_name = current_token_.get_value();
        stmt->set_order_column(column_name);
        // 消耗列名
        advance();

        // 期望 ASC 或 DESC 关键字
        if (match(TokenType::DB_ASC)) {
            stmt->set_order_type(UpdateStmt::OrderType::ASC);
        } else if (match(TokenType::DB_DESC)) {
            stmt->set_order_type(UpdateStmt::OrderType::DESC);
        } else {
            // 如果不指定排序类型，默认使用 ASC
            stmt->set_order_type(UpdateStmt::OrderType::ASC);
        }
    }

    // 解析可选的 LIMIT 子句
    if (match(TokenType::DB_LIMIT)) {
        // 期望一个数字字面量
        if (!check(TokenType::DB_NUMBER_LITERAL)) {
            error("Expected number after LIMIT, but got: " + current_token_.to_string());
        }
        std::string limit_text = current_token_.get_value();
        try {
            long long limit_value = std::stoll(limit_text);
            if (limit_value < 0) {
                error("LIMIT value must be non-negative, but got: " + limit_text);
            }
            std::size_t max_limit = std::numeric_limits<std::size_t>::max();
            if (limit_value > static_cast<long long>(max_limit)) {
                error("LIMIT value too large: " + limit_text);
            }
            stmt->set_limit(static_cast<std::size_t>(limit_value));
        } catch (const std::exception & e) {
            error("Invalid LIMIT value '" + limit_text + "': " + e.what());
        }
        // 消耗数字字面量
        advance();
    }

    // UPDATE 语句解析完成
    return stmt;
}

std::unique_ptr<DeleteStmt> Parser::parse_delete_stmt()
{
    // 获取 DELETE 关键字的位置信息
    std::size_t line = current_token_.get_line();
    std::size_t column = current_token_.get_column();

    // 创建 DeleteStmt 节点
    auto stmt = std::make_unique<DeleteStmt>(line, column);

    // 消耗 DELETE 关键字
    advance();

    // 解析 FROM 关键字
    consume(TokenType::DB_FROM, "Expected FROM after DELETE");

    // 解析集合名
    if (!check(TokenType::DB_IDENTIFIER)) {
        error("Expected collection_name after FROM");
    }
    std::string collection_name = current_token_.get_value();
    stmt->set_collection_name(collection_name);
    // 消耗集合名
    advance();

    // 解析可选的 WHERE 子句
    if (match(TokenType::DB_WHERE)) {
        auto where_expr = parse_expression();
        stmt->set_where_clause(std::move(where_expr));
    }

    // 解析可选的 ORDER BY 子句
    if (match(TokenType::DB_ORDER)) {
        // 期望 BY 关键字
        consume(TokenType::DB_BY, "Expected BY after ORDER");

        // 解析列名
        if (!check(TokenType::DB_IDENTIFIER)) {
            error("Expected column name after ORDER BY");
        }
        std::string column_name = current_token_.get_value();
        stmt->set_order_column(column_name);
        // 消耗列名
        advance();

        // 期望 ASC 或 DESC 关键字
        if (match(TokenType::DB_ASC)) {
            stmt->set_order_type(DeleteStmt::OrderType::ASC);
        } else if (match(TokenType::DB_DESC)) {
            stmt->set_order_type(DeleteStmt::OrderType::DESC);
        } else {
            // 如果不指定排序类型，默认使用 ASC
            stmt->set_order_type(DeleteStmt::OrderType::ASC);
        }
    }

    // 解析可选的 LIMIT 子句
    if (match(TokenType::DB_LIMIT)) {
        // 期望一个数字字面量
        if (!check(TokenType::DB_NUMBER_LITERAL)) {
            error("Expected number after LIMIT, but got: " + current_token_.to_string());
        }
        std::string limit_text = current_token_.get_value();
        try {
            long long limit_value = std::stoll(limit_text);
            if (limit_value < 0) {
                error("LIMIT value must be non-negative, but got: " + limit_text);
            }
            std::size_t max_limit = std::numeric_limits<std::size_t>::max();
            if (limit_value > static_cast<long long>(max_limit)) {
                error("LIMIT value too large: " + limit_text);
            }
            stmt->set_limit(static_cast<std::size_t>(limit_value));
        } catch (const std::exception & e) {
            error("Invalid LIMIT value '" + limit_text + "': " + e.what());
        }
        // 消耗数字字面量
        advance();
    }

    // DELETE 语句解析完成
    return stmt;
}

std::unique_ptr<CreateStmt> Parser::parse_create_stmt()
{    
    // 获取 CREATE 关键字的位置信息
    std::size_t line = current_token_.get_line();
    std::size_t column = current_token_.get_column();
    
    // 创建 CreateStmt 节点
    auto stmt = std::make_unique<CreateStmt>(line, column);
    
    // 消耗 CREATE 关键字
    advance();
    
    // 解析对象类型
    CreateStmt::CreateType create_type;
    if (match(TokenType::DB_DATABASE)) {
        create_type = CreateStmt::CreateType::DATABASE;
    } else if (match(TokenType::DB_COLLECTION)) {
        create_type = CreateStmt::CreateType::COLLECTION;
    } else if (match(TokenType::DB_INDEX)) {
        create_type = CreateStmt::CreateType::INDEX;
    } else if (match(TokenType::DB_VINDEX)) {
        create_type = CreateStmt::CreateType::VINDEX;
    } else {
        error("Expected DATABASE, COLLECTION, INDEX or VINDEX after CREATE");
    }
    stmt->set_create_type(create_type);

    // 查看是否存在 'IF NOT EXISTS' 关键字
    if (match(TokenType::DB_IF)) {
        // 存在 'IF NOT EXISTS' 关键字，解析对象名称
        consume(TokenType::DB_NOT, "Expected NOT after IF");
        consume(TokenType::DB_EXISTS, "Expected EXISTS after NOT");
        stmt->set_is_if_not_exists(true);
    }

    // 解析对象名称
    if (!check(TokenType::DB_IDENTIFIER)) {
        error("Expected object name after " + CreateStmt::create_type_to_string(create_type));
    }
    std::string object_name = current_token_.get_value();
    stmt->set_object_name(object_name);
    // 消耗 object_name
    advance();

    // 根据类型解析不同内容
    if (create_type == CreateStmt::CreateType::COLLECTION) {
        // 如果是 COLLECTION，解析列定义列表
        // 期望 '('
        consume(TokenType::DB_LEFT_PAREN, "Expected '(' after collection name");

        // 解析列定义列表（至少需要一个列定义）
        do {
            ColumnDefinition col_def = parse_column_definition();
            stmt->add_column_definition(std::move(col_def));

            // 如果遇到逗号，继续解析下一个列定义
        } while (match(TokenType::DB_COMMA));

        // 期望 ')'
        consume(TokenType::DB_RIGHT_PAREN, "Expected ')' after column definitions");
    } else if (create_type == CreateStmt::CreateType::INDEX || create_type == CreateStmt::CreateType::VINDEX) {
        // 如果是 INDEX 或 VINDEX，解析集合名
        // 期望 'ON'
        consume(TokenType::DB_ON, "Expected ON after INDEX or VINDEX");

        // 解析集合名
        if (!check(TokenType::DB_IDENTIFIER)) {
            error("Expected collection name after ON");
        }
        std::string collection_name = current_token_.get_value();
        stmt->set_collection_name(collection_name);
        // 消耗 collection_name
        advance();

        // 期望 '('
        consume(TokenType::DB_LEFT_PAREN, "Expected '(' after ON");

        // 解析列名，至少需要一个列名
        do {
            if (!check(TokenType::DB_IDENTIFIER)) {
                error("Expected column name after '('");
            }
            std::string column_name = current_token_.get_value();
            stmt->add_column_name(column_name);
            // 消耗 column_name
            advance();
        } while (match(TokenType::DB_COMMA));

        // 期望 ')'
        consume(TokenType::DB_RIGHT_PAREN, "Expected ')' after column names");

        // 查看是否存在 'USING' 关键字
        if (match(TokenType::DB_USING)) {
            // 存在 'USING' 关键字，根据不同索引类型做解析
            if (create_type == CreateStmt::CreateType::INDEX) {
                // INDEX 索引类型
                // 期望 'BTREE' 或 'HASH'
                if (check(TokenType::DB_BTREE)) {
                    stmt->set_index_type(IndexType::BTREE);
                } else if (check(TokenType::DB_HASH)) {
                    stmt->set_index_type(IndexType::HASH);
                } else {
                    error("Expected BTREE or HASH after USING");
                }
                // 消耗 index_type
                advance();
            } else {
                // VINDEX 索引类型
                // 期望 'FLAT' 或 'IVF_FLAT' 或 'HNSW'
                if (check(TokenType::DB_FLAT)) {
                    stmt->set_vindex_type(VIndexType::FLAT);
                } else if (check(TokenType::DB_IVF_FLAT)) {
                    stmt->set_vindex_type(VIndexType::IVF_FLAT);
                } else if (check(TokenType::DB_HNSW)) {
                    stmt->set_vindex_type(VIndexType::HNSW);
                } else {
                    error("Expected FLAT, IVF_FLAT or HNSW after USING");
                }
                // 消耗 vindex_type
                advance();

                // 查看是否存在 'WITH' 关键字
                if (match(TokenType::DB_WITH)) {
                    // 存在 'WITH' 关键字，解析 WITH 子句
                    // 期望 '('
                    consume(TokenType::DB_LEFT_PAREN, "Expected '(' after WITH");

                    // 解析 WITH 子句
                    VIndexWithClause with_clause;
                    parse_vindex_with_clause(with_clause);
                    stmt->set_vindex_with_clause(std::move(with_clause));

                    // 期望 ')'
                    consume(TokenType::DB_RIGHT_PAREN, "Expected ')' after WITH");
                } else {
                    // 不存在 'WITH' 关键字，使用默认参数
                    VIndexWithClause with_clause;
                    stmt->set_vindex_with_clause(std::move(with_clause));
                }
            }
        } else {
            // 不存在 'USING' 关键字，使用默认索引类型
            if (create_type == CreateStmt::CreateType::INDEX) {
                // INDEX 索引类型，默认使用 B-Tree 索引
                stmt->set_index_type(IndexType::BTREE);
            } else {
                // VINDEX 索引类型，默认使用 FLAT 索引
                stmt->set_vindex_type(VIndexType::FLAT);

                // 查看是否存在 'WITH' 关键字
                if (match(TokenType::DB_WITH)) {
                    // 存在 'WITH' 关键字，解析 WITH 子句
                    // 期望 '('
                    consume(TokenType::DB_LEFT_PAREN, "Expected '(' after WITH");

                    // 解析 WITH 子句
                    VIndexWithClause with_clause;
                    parse_vindex_with_clause(with_clause);
                    stmt->set_vindex_with_clause(std::move(with_clause));

                    // 期望 ')'
                    consume(TokenType::DB_RIGHT_PAREN, "Expected ')' after WITH");
                } else {
                    // 不存在 'WITH' 关键字，使用默认参数
                    VIndexWithClause with_clause;
                    stmt->set_vindex_with_clause(std::move(with_clause));
                }
            }
        }
    }

    // 如果是 DATABASE，不需要解析其他内容

    // CREATE 语句解析完成
    return stmt;
}

std::unique_ptr<DropStmt> Parser::parse_drop_stmt()
{
    // 获取 DROP 关键字的位置信息
    std::size_t line = current_token_.get_line();
    std::size_t column = current_token_.get_column();

    // 创建 DropStmt 节点
    auto stmt = std::make_unique<DropStmt>(line, column);

    // 消耗 DROP 关键字
    advance();

    // 解析对象类型
    DropStmt::DropType drop_type;
    if (match(TokenType::DB_DATABASE)) {
        drop_type = DropStmt::DropType::DATABASE;
    } else if (match(TokenType::DB_COLLECTION)) {
        drop_type = DropStmt::DropType::COLLECTION;
    } else if (match(TokenType::DB_INDEX)) {
        drop_type = DropStmt::DropType::INDEX;
    } else if (match(TokenType::DB_VINDEX)) {
        drop_type = DropStmt::DropType::VINDEX;
    } else {
        error("Expected DATABASE, COLLECTION, INDEX or VINDEX after DROP");
    }

    stmt->set_drop_type(drop_type);

    // 解析对象名称
    if (!check(TokenType::DB_IDENTIFIER)) {
        error("Expected object_name after DROP" + DropStmt::drop_type_to_string(drop_type));
    }
    std::string object_name = current_token_.get_value();
    // 消耗 object_name
    advance();

    stmt->set_object_name(object_name);    

    // DROP 语句解析完成
    return stmt;
}

std::unique_ptr<UseStmt> Parser::parse_use_stmt()
{
    // 获取 USE 关键字的位置信息
    std::size_t line = current_token_.get_line();
    std::size_t column = current_token_.get_column();

    // 创建 UseStmt 节点
    auto stmt = std::make_unique<UseStmt>(line, column);

    // 消耗 USE 关键字
    advance();

    // 解析数据库名称
    if (!check(TokenType::DB_IDENTIFIER)) {
        error("Expected database_name after USE");
    }
    std::string database_name = current_token_.get_value();
    // 消耗 database_name
    advance();

    stmt->set_database_name(database_name);

    // USE 语句解析完成
    return stmt;
}

std::unique_ptr<AlterStmt> Parser::parse_alter_stmt()
{
    // 获取 ALTER 关键字的位置信息
    std::size_t line = current_token_.get_line();
    std::size_t column = current_token_.get_column();

    // 创建 AlterStmt 节点
    auto stmt = std::make_unique<AlterStmt>(line, column);

    // 消耗 ALTER 关键字
    advance();

    // 期望 'COLLECTION'
    consume(TokenType::DB_COLLECTION, "Expected COLLECTION after ALTER");

    // 解析集合名称
    if (!check(TokenType::DB_IDENTIFIER)) {
        error("Expected collection_name after ALTER");
    }
    std::string collection_name = current_token_.get_value();
    stmt->set_collection_name(collection_name);
    // 消耗 collection_name
    advance();

    // 解析 ALTER 类型
    AlterStmt::AlterType alter_type;
    if (match(TokenType::DB_ADD)) {
        alter_type = AlterStmt::AlterType::ADD_COLUMN;
    } else if (match(TokenType::DB_DROP)) {
        alter_type = AlterStmt::AlterType::DROP_COLUMN;
    } else if (match(TokenType::DB_MODIFY)) {
        alter_type = AlterStmt::AlterType::MODIFY_COLUMN;
    } else if (match(TokenType::DB_RENAME)) {
        alter_type = AlterStmt::AlterType::RENAME_COLUMN;
    } else {
        error("Expected ADD, DROP, MODIFY or RENAME after ALTER");
    }

    stmt->set_alter_type(alter_type);

    // 期望 'COLUMN'
    consume(TokenType::DB_COLUMN, "Expected COLUMN after ADD/MODIFY/RENAME/DROP");

    // 根据 ALTER 类型解析不同内容
    if (alter_type == AlterStmt::AlterType::DROP_COLUMN) {
        // 解析字段名称
        if (!check(TokenType::DB_IDENTIFIER)) {
            error("Expected column_name after COLUMN");
        }
        std::string old_column_name = current_token_.get_value();
        // 消耗 old_column_name
        advance();
        stmt->set_old_column_name(old_column_name);
    } else if (alter_type == AlterStmt::AlterType::RENAME_COLUMN) {
        // 解析旧字段名称
        if (!check(TokenType::DB_IDENTIFIER)) {
            error("Expected column_name after COLUMN");
        }
        std::string old_column_name = current_token_.get_value();
        // 消耗 old_column_name
        advance();
        stmt->set_old_column_name(old_column_name);

        // 期望 'TO'
        consume(TokenType::DB_TO, "Expected TO after RENAME");

        // 解析新字段名称
        if (!check(TokenType::DB_IDENTIFIER)) {
            error("Expected column_name after RENAME");
        }
        std::string new_column_name = current_token_.get_value();
        // 消耗 new_column_name
        advance();
        stmt->set_column_name(new_column_name);
    } else if (alter_type == AlterStmt::AlterType::ADD_COLUMN) {
        // 解析字段定义
        ColumnDefinition column_definition = parse_column_definition();
        stmt->set_new_column_definition(std::move(column_definition));
    } else if (alter_type == AlterStmt::AlterType::MODIFY_COLUMN) {
        // 解析字段定义
        // 旧的字段名直接解析到 column_definition 中
        ColumnDefinition column_definition = parse_column_definition();
        stmt->set_new_column_definition(std::move(column_definition));

        // 保存旧的字段名
        stmt->set_old_column_name(column_definition.get_name());
    }

    // ALTER 语句解析完成
    return stmt;
}

std::unique_ptr<ShowStmt> Parser::parse_show_stmt()
{
    // 获取 SHOW 关键字的位置信息
    std::size_t line = current_token_.get_line();
    std::size_t column = current_token_.get_column();

    // 创建 ShowStmt 节点
    auto stmt = std::make_unique<ShowStmt>(line, column);

    // 消耗 SHOW 关键字
    advance();

    // 解析对象类型
    ShowStmt::ShowType show_type;
    if (match(TokenType::DB_DATABASES)) {
        show_type = ShowStmt::ShowType::DATABASES;
    } else if (match(TokenType::DB_COLLECTIONS)) {
        show_type = ShowStmt::ShowType::COLLECTIONS;
    } else if (match(TokenType::DB_INDEXES)) {
        show_type = ShowStmt::ShowType::INDEXES;
    } else if (match(TokenType::DB_VINDEXES)) {
        show_type = ShowStmt::ShowType::VINDEXES;
    } else {
        error("Expected DATABASES, COLLECTIONS or INDEXES after SHOW");
    }

    stmt->set_show_type(show_type);

    if (show_type == ShowStmt::ShowType::INDEXES || show_type == ShowStmt::ShowType::VINDEXES) {
        // 索引类型需要继续识别对象
        consume(TokenType::DB_FROM, "Expected FROM after INDEXES or VINDEXES");

        if (!check(TokenType::DB_IDENTIFIER)) {
            error("Expected collection_name after FROM");
        }
        std::string collection_name = current_token_.get_value();
        stmt->set_collection_name(collection_name);
        // 消耗 collection_name
        advance();
    }

    // SHOW 语句解析完成
    return stmt;
}

std::unique_ptr<DescribeStmt> Parser::parse_describe_stmt()
{
    // 获取 DESCRIBE 关键字的位置信息
    std::size_t line = current_token_.get_line();
    std::size_t column = current_token_.get_column();

    // 创建 DescribeStmt 节点
    auto stmt = std::make_unique<DescribeStmt>(line, column);

    // 消耗 DESCRIBE 关键字
    advance();

    // 消耗 COLLECTION 关键字
    consume(TokenType::DB_COLLECTION, "Expected COLLECTION after DESCRIBE or DESC");

    // 解析集合名称
    if (!check(TokenType::DB_IDENTIFIER)) {
        error("Expected collection_name after DESCRIBE");
    }
    std::string collection_name = current_token_.get_value();
    // 消耗 collection_name
    advance();

    stmt->set_collection_name(collection_name);

    // DESCRIBE 语句解析完成
    return stmt;
}

std::unique_ptr<AstNode> Parser::parse_expression()
{
    // 该接口只是表达式解析入口，具体解析逻辑在各个解析函数中实现
    // 递归下降解析器会自动处理所有优先级

    // 表达式优先级：
    // 1. DB_OR          (最低优先级，如: a DB_OR b)
    // 2. DB_AND         (如: a DB_AND b)
    // 3. 比较运算符   (如: =, !=, <, >, <=, >=)
    // 4. 加减        (如: +, -)
    // 5. 乘除        (如: *, /, %)
    // 6. 一元运算符   (如: +, -, DB_NOT)
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
    switch (current_token_.get_type()) {
        // 字面量：数字、字符串、布尔值
        case TokenType::DB_NUMBER_LITERAL:
        case TokenType::DB_STRING_LITERAL:
        case TokenType::DB_TRUE:
        case TokenType::DB_FALSE:
        // 标识符：字段名、变量等
        case TokenType::DB_IDENTIFIER:
        // 一元运算符：+、-、NOT
        case TokenType::DB_PLUS:
        case TokenType::DB_MINUS:
        case TokenType::DB_NOT:
        // 括号表达式
        case TokenType::DB_LEFT_PAREN:
            // 调用最低优先级解析函数
            return parse_or_expression();
        default:
            // 非法的表达式开始 token
            error("Expected expression, but got: " + current_token_.to_string());
            return nullptr;
    }
}

std::unique_ptr<AstNode> Parser::parse_or_expression()
{
    // 解析左侧的 DB_AND 表达式
    auto left = parse_and_expression();
    
    // 循环处理多个 DB_OR 运算符（左结合）
    // 例如: a DB_OR b DB_OR c 解析为 ((a DB_OR b) DB_OR c)
    while (check(TokenType::DB_OR)) {
        // 保存 DB_OR token 的位置信息
        std::size_t line = current_token_.get_line();
        std::size_t column = current_token_.get_column();
        
        // 消耗 DB_OR token
        advance();
        
        // 创建二元表达式节点
        auto expr = std::make_unique<BinaryExpr>(line, column);
        
        // 设置运算符类型为 DB_OR
        expr->set_op_type(BinaryOperatorType::DB_OR);
        
        // 设置左操作数为之前解析的结果
        expr->set_left(std::move(left));
        
        // 解析右侧的 DB_AND 表达式
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
    
    // 循环处理多个 DB_AND 运算符（左结合）
    // 例如: a DB_AND b DB_AND c 解析为 ((a DB_AND b) DB_AND c)
    while (check(TokenType::DB_AND)) {
        // 保存 DB_AND token 的位置信息
        std::size_t line = current_token_.get_line();
        std::size_t column = current_token_.get_column();
        
        // 消耗 DB_AND token
        advance();
        
        // 创建二元表达式节点
        auto expr = std::make_unique<BinaryExpr>(line, column);
        
        // 设置运算符类型为 DB_AND
        expr->set_op_type(BinaryOperatorType::DB_AND);
        
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
    switch (current_token_.get_type()) {
        case TokenType::DB_EQUAL:
            line = current_token_.get_line();
            column = current_token_.get_column();
            op_type = BinaryOperatorType::DB_EQUAL;
            has_comparison = true;
            advance();
            break;
        case TokenType::DB_NOT_EQUAL:
            line = current_token_.get_line();
            column = current_token_.get_column();
            op_type = BinaryOperatorType::DB_NOT_EQUAL;
            has_comparison = true;
            advance();
            break;
        case TokenType::DB_LESS_THAN:
            line = current_token_.get_line();
            column = current_token_.get_column();
            op_type = BinaryOperatorType::DB_LESS_THAN;
            has_comparison = true;
            advance();
            break;
        case TokenType::DB_GREATER_THAN:
            line = current_token_.get_line();
            column = current_token_.get_column();
            op_type = BinaryOperatorType::DB_GREATER_THAN;
            has_comparison = true;
            advance();
            break;
        case TokenType::DB_LESS_EQUAL:
            line = current_token_.get_line();
            column = current_token_.get_column();
            op_type = BinaryOperatorType::DB_LESS_EQUAL;
            has_comparison = true;
            advance();
            break;
        case TokenType::DB_GREATER_EQUAL:
            line = current_token_.get_line();
            column = current_token_.get_column();
            op_type = BinaryOperatorType::DB_GREATER_EQUAL;
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
    while (check(TokenType::DB_PLUS) || check(TokenType::DB_MINUS)) {
        // 保存运算符信息
        TokenType op_token_type = current_token_.get_type();
        std::size_t line = current_token_.get_line();
        std::size_t column = current_token_.get_column();

        // 消耗运算符
        advance();

        // 创建二元表达式节点
        auto expr = std::make_unique<BinaryExpr>(line, column);

        // 设置运算符类型
            if (op_token_type == TokenType::DB_PLUS) {
                expr->set_op_type(BinaryOperatorType::DB_PLUS);
        } else {
            expr->set_op_type(BinaryOperatorType::DB_MINUS);
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
    while (check(TokenType::DB_MULTIPLY) ||
           check(TokenType::DB_DIVIDE) ||
           check(TokenType::DB_MODULO)) {
        // 保存运算符信息
        TokenType op_token_type = current_token_.get_type();
        std::size_t line = current_token_.get_line();
        std::size_t column = current_token_.get_column();

        // 消耗运算符
        advance();

        // 创建二元表达式节点
        auto expr = std::make_unique<BinaryExpr>(line, column);

        // 设置运算符类型
        switch (op_token_type) {
            case TokenType::DB_MULTIPLY:
                expr->set_op_type(BinaryOperatorType::DB_MULTIPLY);
                break;
            case TokenType::DB_DIVIDE:
                expr->set_op_type(BinaryOperatorType::DB_DIVIDE);
                break;
            case TokenType::DB_MODULO:
                expr->set_op_type(BinaryOperatorType::DB_MODULO);
                break;
            default:
                // 理论上不会到这里
                error("Unexpected token in multiplicative expression: " + current_token_.to_string());
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
    if (check(TokenType::DB_NOT) ||
        check(TokenType::DB_PLUS) ||
        check(TokenType::DB_MINUS)) {
        TokenType op_token_type = current_token_.get_type();
        std::size_t line = current_token_.get_line();
        std::size_t column = current_token_.get_column();

        // 消耗一元运算符
        advance();

        auto expr = std::make_unique<UnaryExpr>(line, column);

        // 设置运算符类型
        switch (op_token_type) {
            case TokenType::DB_NOT:
                expr->set_op_type(UnaryOperatorType::DB_NOT);
                break;
            case TokenType::DB_PLUS:
                expr->set_op_type(UnaryOperatorType::DB_PLUS);
                break;
            case TokenType::DB_MINUS:
                expr->set_op_type(UnaryOperatorType::DB_MINUS);
                break;
            default:
                // 理论上不会到这里
                error("Unexpected token in unary expression: " + current_token_.to_string());
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
    if (check(TokenType::DB_LEFT_PAREN)) {
        // 消耗 '('
        advance();

        // 解析括号中的表达式
        auto expr = parse_expression();

        // 期望并消耗 ')'
        consume(TokenType::DB_RIGHT_PAREN, "Expected ')' after expression");
        return expr;
    }

    // 向量字面量，格式为 [1, 2, 3]
    if (check(TokenType::DB_LEFT_BRACKET)) {
        return parse_vector_literal();
    }

    // 标识符：可能是函数调用或普通标识符
    if (check(TokenType::DB_IDENTIFIER)) {
        // 通过 peek_token 判断是否为函数调用
        Token next = lexer_->peek_token();
        if (next.get_type() == TokenType::DB_LEFT_PAREN) {
            // 发现 '('，尝试解析为函数调用
            return parse_function_call();
        } else {
            // 否则解析为普通标识符
            return parse_identifier_expr();
        }
    }

    // 字面量：数字、字符串、布尔值、NULL
    if (check(TokenType::DB_NUMBER_LITERAL) ||
        check(TokenType::DB_STRING_LITERAL) ||
        check(TokenType::DB_TRUE) ||
        check(TokenType::DB_FALSE) ||
        check(TokenType::DB_NULL)) {
        return parse_literal_expr();
    }

    // 其他情况都是错误
    error("Expected primary expression, but got: " + current_token_.to_string());
}

std::unique_ptr<FunctionCallExpr> Parser::parse_function_call()
{
    // 获取函数名、位置信息
    std::size_t line = current_token_.get_line();
    std::size_t column = current_token_.get_column();
    std::string function_name = current_token_.get_value();

    // 消耗函数名
    advance();

    // 期望 '('
    consume(TokenType::DB_LEFT_PAREN, "Expected '(' after function name");

    // 创建函数调用表达式节点
    auto func = std::make_unique<FunctionCallExpr>(line, column);
    func->set_function_name(function_name);

    // 处理无参数情况：立即遇到 ')'
    if (check(TokenType::DB_RIGHT_PAREN)) {
        // 消耗 ')'
        advance();
        // 函数调用表达式解析完毕
        return func;
    }

    // 解析参数列表
    do {
        func->add_argument(parse_expression());
    } while (match(TokenType::DB_COMMA));

    // 期望 ')'
    consume(TokenType::DB_RIGHT_PAREN, "Expected ')' after function arguments");

    // 函数调用表达式解析完毕
    return func;
}

std::unique_ptr<IdentifierExpr> Parser::parse_identifier_expr()
{
    // 获取标识符位置信息和第一个标识符部分
    std::size_t line = current_token_.get_line();
    std::size_t column = current_token_.get_column();
    std::string first_part = current_token_.get_value();

    // 创建标识符表达式节点
    auto ident = std::make_unique<IdentifierExpr>(line, column);
    // 标识符类型通过默认构造设置为 COLUMN
    ident->add_part(first_part);
    ident->set_original_identifier(first_part);

    // 消耗第一个标识符
    advance();

    // 处理限定名：schema.collection.column
    while (match(TokenType::DB_DOT)) {
        // 期望标识符
        if (!check(TokenType::DB_IDENTIFIER)) {
            error("Expected identifier after '.', but got: " + current_token_.to_string());
        }
        std::string next_part = current_token_.get_value();
        ident->add_part(next_part);
        // 更新 original_identifier 表示完整限定名
        ident->set_original_identifier(ident->get_original_identifier() + "." + next_part);

        // 消耗标识符
        advance();
    }

    // 标识符表达式解析完毕
    return ident;
}

std::unique_ptr<LiteralExpr> Parser::parse_literal_expr()
{
    // 根据当前 token 类型解析字面量
    switch (current_token_.get_type()) {
        case TokenType::DB_STRING_LITERAL:
            return parse_string_literal();
        case TokenType::DB_NUMBER_LITERAL:
            return parse_number_literal();
        case TokenType::DB_TRUE:
        case TokenType::DB_FALSE:
            return parse_boolean_literal();
        case TokenType::DB_NULL:
            return parse_null_literal();
        default:
            error("Expected literal expression, but got: " + current_token_.to_string());
    }
}

std::unique_ptr<LiteralExpr> Parser::parse_string_literal()
{
    // 获取字符串字面量位置信息和字符串值
    std::size_t line = current_token_.get_line();
    std::size_t column = current_token_.get_column();
    std::string value = current_token_.get_value();

    // 创建字符串字面量表达式节点
    auto literal = std::make_unique<LiteralExpr>(line, column);
    literal->set_literal_type(LiteralExpr::LiteralType::STRING);
    literal->set_literal_value(LiteralExpr::LiteralValue{value});

    // 消耗字符串字面量
    advance();

    // 字符串字面量解析完毕
    return literal;
}

std::unique_ptr<LiteralExpr> Parser::parse_number_literal()
{
    // 获取数字字面量位置信息和数字值
    std::size_t line = current_token_.get_line();
    std::size_t column = current_token_.get_column();
    const std::string & text = current_token_.get_value();

    // 创建数字字面量表达式节点
    auto literal = std::make_unique<LiteralExpr>(line, column);

    // 解析数字字面量
    try {
        // 判断是否为浮点数，暂不支持科学计数法解析，如需支持，需要修改 Lexer::read_number()
        // Lexer 确保如果存在小数点，则必然存在小数部分
        bool is_float = (text.find('.') != std::string::npos);

        if (is_float) {
            double v = std::stod(text);
            literal->set_literal_type(LiteralExpr::LiteralType::FLOAT);
            literal->set_literal_value(LiteralExpr::LiteralValue{v});
        } else {
            long long v = std::stoll(text);
            literal->set_literal_type(LiteralExpr::LiteralType::INTEGER);
            literal->set_literal_value(LiteralExpr::LiteralValue{static_cast<std::int64_t>(v)});
        }
    } catch (const std::exception & e) {
        error(std::string("Invalid number literal '") + text + "': " + e.what());
    }

    // 消耗数字字面量
    advance();

    // 数字字面量解析完毕
    return literal;
}

std::unique_ptr<LiteralExpr> Parser::parse_boolean_literal()
{
    // 获取布尔字面量位置信息和布尔值
    std::size_t line = current_token_.get_line();
    std::size_t column = current_token_.get_column();

    // 根据 token 类型确定布尔值
    bool value;
    if (current_token_.get_type() == TokenType::DB_TRUE) {
        value = true;
    } else if (current_token_.get_type() == TokenType::DB_FALSE) {
        value = false;
    } else {
        error("Expected TRUE or FALSE, but got: " + current_token_.to_string());
    }

    // 创建布尔字面量表达式节点
    auto literal = std::make_unique<LiteralExpr>(line, column);
    literal->set_literal_type(LiteralExpr::LiteralType::BOOLEAN);
    literal->set_literal_value(LiteralExpr::LiteralValue{value});

    // 消耗布尔字面量
    advance();

    // 布尔字面量解析完毕
    return literal;
}

std::unique_ptr<LiteralExpr> Parser::parse_null_literal()
{
    // 获取空值字面量位置信息
    std::size_t line = current_token_.get_line();
    std::size_t column = current_token_.get_column();

    // 创建空值字面量表达式节点
    auto literal = std::make_unique<LiteralExpr>(line, column);
    literal->set_literal_type(LiteralExpr::LiteralType::NULL_VALUE);
    literal->set_literal_value(LiteralExpr::LiteralValue{Null{}});

    // 消耗空值字面量
    advance();

    // 空值字面量解析完毕
    return literal;
}

std::unique_ptr<LiteralExpr> Parser::parse_vector_literal()
{
    // 获取向量字面量位置信息和向量值
    std::size_t line = current_token_.get_line();
    std::size_t column = current_token_.get_column();

    // 创建向量字面量表达式节点
    auto literal = std::make_unique<LiteralExpr>(line, column);

    // 开始解析向量字面量
    // 期望 '['
    consume(TokenType::DB_LEFT_BRACKET, "Expected '[' at start of vector literal");

    // 解析向量元素列表
    std::vector<float> values;

    // 如果下一个字符是 ']'，则处理空向量的情况
    if (check(TokenType::DB_RIGHT_BRACKET)) {
        // 消耗 ']'
        advance();

        // 设置向量类型和值
        literal->set_literal_type(LiteralExpr::LiteralType::VECTOR);
        literal->set_literal_value(LiteralExpr::LiteralValue{values});
        return literal;
    }

    // 解析向量元素，至少包含一个元素
    do {
        // 期望数字字面量
        if (!check(TokenType::DB_NUMBER_LITERAL)) {
            error("Expected number in vector literal, but got: " + current_token_.to_string());
        }

        // 解析数字
        const std::string & number_text = current_token_.get_value();
        try {
            // 向量元素必须是浮点数（即使输入是整数，也转换为浮点数）
            double v = std::stod(number_text);
            values.push_back(static_cast<float>(v));
        } catch (const std::exception & e) {
            error(std::string("Invalid number in vector literal '") + number_text + "': " + e.what());
        }

        // 消耗数字字面量
        advance();

        // 如果遇到 ']'，结束解析
        if (check(TokenType::DB_RIGHT_BRACKET)) {
            break;
        }

        // 期望 ',' 分隔符
        consume(TokenType::DB_COMMA, "Expected ',' or ']' after vector element");

        // 如果遇到 ']'，结束解析（处理末尾逗号的情况）
        if (check(TokenType::DB_RIGHT_BRACKET)) {
            break;
        }

    } while (true);

    // 期望 ']'
    consume(TokenType::DB_RIGHT_BRACKET, "Expected ']' at end of vector literal");

    // 设置向量类型和值
    literal->set_literal_type(LiteralExpr::LiteralType::VECTOR);
    literal->set_literal_value(LiteralExpr::LiteralValue{values});

    // 向量字面量解析完毕
    return literal;
}

ColumnDefinition Parser::parse_column_definition()
{
    ColumnDefinition col_def;

    // 解析列名
    if (!check(TokenType::DB_IDENTIFIER)) {
        error("Expected column name, but got: " + current_token_.to_string());
    }
    std::string column_name = current_token_.get_value();
    col_def.set_name(column_name);
    // 消耗列名
    advance();
    
    // 解析字段类型
    FieldType field_type = parse_field_type();
    col_def.set_type(field_type);

    // 解析类型的长度、精度参数，详情见 SQL 功能参考手册
    switch (field_type) {
        case FieldType::CHAR:
        case FieldType::VARCHAR:
        case FieldType::VECTOR: {
            // 只包含一个参数的类型
            // 期望 '('
            consume(TokenType::DB_LEFT_PAREN, "Expected '(' after type");

            // 解析长度参数
            if (!check(TokenType::DB_NUMBER_LITERAL)) {
                error("Expected number in type parameter, but got: " + current_token_.to_string());
            }
            int length = std::stoi(current_token_.get_value());
            col_def.set_length(length);
            // 消耗长度参数
            advance();

            // 期望 ')'
            consume(TokenType::DB_RIGHT_PAREN, "Expected ')' after type parameter");
            break;
        }
        case FieldType::DECIMAL: {
            // 包含两个参数的类型
            // 期望 '('
            consume(TokenType::DB_LEFT_PAREN, "Expected '(' after type");

            // 解析长度参数
            if (!check(TokenType::DB_NUMBER_LITERAL)) {
                error("Expected number in type parameter, but got: " + current_token_.to_string());
            }
            int length = std::stoi(current_token_.get_value());
            col_def.set_length(length);
            // 消耗长度参数
            advance();

            // 期望 ','
            consume(TokenType::DB_COMMA, "Expected ',' after length parameter");

            // 解析精度参数
            if (!check(TokenType::DB_NUMBER_LITERAL)) {
                error("Expected number in type parameter, but got: " + current_token_.to_string());
            }
            int precision = std::stoi(current_token_.get_value());
            col_def.set_precision(precision);
            // 消耗精度参数
            advance();

            // 期望 ')'
            consume(TokenType::DB_RIGHT_PAREN, "Expected ')' after type parameter");
            break;
        }
        case FieldType::ENUM: {
            // 包含多个参数的类型
            // 期望 '('
            consume(TokenType::DB_LEFT_PAREN, "Expected '(' after type");

            // 解析选项
            std::vector<std::string> options;
            do {
                if (!check(TokenType::DB_IDENTIFIER)) {
                    error("Expected identifier in options, but got: " + current_token_.to_string());
                }
                std::string option = current_token_.get_value();
                options.push_back(option);
                // 消耗选项
                advance();
            } while (match(TokenType::DB_COMMA));
            col_def.set_options(std::move(options));

            // 期望 ')'
            consume(TokenType::DB_RIGHT_PAREN, "Expected ')' after type parameter");
            break;
        }
        default:
            break;
    }

    // 解析列约束，列约束可以以任意顺序出现
    while (true) {
        if (match(TokenType::DB_NOT)) {
            // NOT NULL
            consume(TokenType::DB_NULL, "Expected NULL after NOT");
            col_def.set_is_nullable(false);
        } else if (match(TokenType::DB_PRIMARY)) {
            // PRIMARY KEY
            consume(TokenType::DB_KEY, "Expected KEY after PRIMARY");
            col_def.set_is_primary(true);
        } else if (match(TokenType::DB_AUTO_INCREMENT)) {
            // AUTO_INCREMENT
            col_def.set_is_auto_increment(true);
        } else if (match(TokenType::DB_DEFAULT)) {
            // DEFAULT value
            auto default_expr = parse_expression();
            col_def.set_default_value(std::move(default_expr));
        } else {
            // 没有更多属性，退出循环
            break;
        }
    }
    
    return col_def;
}

FieldType Parser::parse_field_type()
{
    // 解析字段类型关键字
    if (!check(TokenType::DB_IDENTIFIER)) {
        error("Expected field type, but got: " + current_token_.to_string());
    }

    std::string type_name = current_token_.get_value();
    // 转为大写进行比较
    std::transform(type_name.begin(), type_name.end(), type_name.begin(),
    [](unsigned char c) {
        return static_cast<char>(std::toupper(c));
    });
    // 消耗类型关键字
    advance();

    // 映射类型名称到 FieldType
    if (type_name == "TINYINT") {
        return FieldType::TINYINT;
    } else if (type_name == "SMALLINT") {
        return FieldType::SMALLINT;
    } else if (type_name == "INT" || type_name == "INTEGER") {
        return FieldType::INTEGER;
    } else if (type_name == "BIGINT") {
        return FieldType::BIGINT;
    } else if (type_name == "FLOAT") {
        return FieldType::FLOAT;
    } else if (type_name == "DOUBLE") {
        return FieldType::DOUBLE;
    } else if (type_name == "DECIMAL") {
        return FieldType::DECIMAL;
    } else if (type_name == "CHAR") {
        return FieldType::CHAR;
    } else if (type_name == "VARCHAR") {
        return FieldType::VARCHAR;
    } else if (type_name == "BOOLEAN" || type_name == "BOOL") {
        return FieldType::BOOLEAN;
    } else if (type_name == "TIMESTAMP") {
        return FieldType::TIMESTAMP;
    } else if (type_name == "ENUM") {
        return FieldType::ENUM;
    } else if (type_name == "VECTOR") {
        return FieldType::VECTOR;
    } else {
        error("Unknown field type: " + type_name);
    }
}

void Parser::parse_vindex_with_clause(VIndexWithClause & with_clause)
{
    do {
        // 解析参数名
        if (!check(TokenType::DB_IDENTIFIER)) {
            error("Expected parameter name in WITH clause, but got: " + current_token_.to_string());
        }
        std::string param_name = current_token_.get_value();
        // 转为小写进行比较
        std::transform(param_name.begin(), param_name.end(), param_name.begin(),
            [](unsigned char c) {
                return static_cast<char>(std::tolower(c));
            }
        );
        // 消耗参数名
        advance();
        
        // 解析 '='
        consume(TokenType::DB_EQUAL, "Expected '=' after parameter name in WITH clause");
        
        // 根据参数名解析对应的值
        if (param_name == "nlist") {
            // nlist 必须是整数
            if (!check(TokenType::DB_NUMBER_LITERAL)) {
                error("Expected number for nlist parameter, but got: " + current_token_.to_string());
            }
            int nlist_value = std::stoi(current_token_.get_value());
            with_clause.nlist = static_cast<std::int32_t>(nlist_value);
            // 消耗数字
            advance();
        } else if (param_name == "m") {
            // M 必须是整数
            if (!check(TokenType::DB_NUMBER_LITERAL)) {
                error("Expected number for M parameter, but got: " + current_token_.to_string());
            }
            int m_value = std::stoi(current_token_.get_value());
            with_clause.M = static_cast<std::int32_t>(m_value);
            // 消耗数字
            advance();
        } else if (param_name == "ef_construction") {
            // ef_construction 必须是整数
            if (!check(TokenType::DB_NUMBER_LITERAL)) {
                error("Expected number for ef_construction parameter, but got: " + current_token_.to_string());
            }
            int ef_value = std::stoi(current_token_.get_value());
            with_clause.ef_construction = static_cast<std::int32_t>(ef_value);
            // 消耗数字
            advance();
        } else if (param_name == "metric") {
            // metric 必须是字符串
            if (!check(TokenType::DB_STRING_LITERAL)) {
                error("Expected string for metric parameter, but got: " + current_token_.to_string());
            }
            std::string metric_str = current_token_.get_value();
            // 转为大写进行比较
            std::transform(metric_str.begin(), metric_str.end(), metric_str.begin(),
                [](unsigned char c) {
                    return static_cast<char>(std::toupper(c));
                }
            );

            // 转换为 MetricType
            if (metric_str == "L2") {
                with_clause.metric = MetricType::L2;
            } else if (metric_str == "IP") {
                with_clause.metric = MetricType::IP;
            } else if (metric_str == "COSINE") {
                with_clause.metric = MetricType::COSINE;
            } else {
                error("Invalid metric value: " + metric_str + ". Expected L2, IP, or COSINE");
            }
            // 消耗字符串
            advance();
        } else {
            error("Unknown parameter name in WITH clause: " + param_name + ". Expected nlist, M, ef_construction, or metric");
        }

        // 如果遇到 ','，继续解析下一个参数
    } while (match(TokenType::DB_COMMA));
}

} // namespace dreamdb
