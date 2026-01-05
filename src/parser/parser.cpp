#include "dreamdb/parser/parser.h"

#include "dreamdb/parser/ast/ast_select_statement_node.h"
#include "dreamdb/parser/ast/ast_insert_statement_node.h"
#include "dreamdb/parser/ast/ast_update_statement_node.h"
#include "dreamdb/parser/ast/ast_delete_statement_node.h"
#include "dreamdb/parser/ast/ast_drop_statement_node.h"
#include "dreamdb/parser/ast/ast_create_statement_node.h"
#include "dreamdb/parser/ast/ast_alter_statement_node.h"
#include "dreamdb/parser/ast/ast_use_statement_node.h"
#include "dreamdb/parser/ast/ast_show_statement_node.h"
#include "dreamdb/parser/ast/ast_describe_statement_node.h"
#include "dreamdb/parser/ast/ast_binary_expression_node.h"
#include "dreamdb/parser/ast/ast_unary_expression_node.h"
#include "dreamdb/parser/ast/ast_in_expression_node.h"
#include "dreamdb/parser/ast/ast_between_expression_node.h"
#include "dreamdb/parser/ast/ast_like_expression_node.h"
#include "dreamdb/parser/ast/ast_literal_expression_node.h"
#include "dreamdb/parser/ast/ast_vector_expression_node.h"
#include "dreamdb/parser/ast/ast_column_reference_expression_node.h"
#include "dreamdb/parser/ast/ast_function_call_expression_node.h"

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
    , current_token_(TokenType::DB_EOF)
{
}

Parser::Parser(std::unique_ptr<Lexer> lexer)
    : lexer_(std::move(lexer))
    , current_token_(TokenType::DB_EOF)
{
}

std::unique_ptr<AstStatementNode> Parser::parse()
{
    // 初始化：读取第一个 Token
    current_token_ = lexer_->next_token();

    // 检查是否为空输入
    if (check(TokenType::DB_EOF)) {
        // do nothing
        return nullptr;
    }

    // 解析语句
    auto statement = parse_statement();

    // 跳过可能存在的分号
    skip_semicolon();

    // 检查是否还有更多内容
    if (!check(TokenType::DB_EOF)) {
        error("Unexpected token");
        return nullptr;
    }

    // 解析完毕
    return statement;
}

std::unique_ptr<AstStatementNode> Parser::parse_statement()
{
    // 根据当前 Token 类型选择对应的语句解析函数
    switch (current_token_.get_type()) {
        case TokenType::DB_SELECT:
            return parse_select_statement();
        case TokenType::DB_INSERT:
            return parse_insert_statement();
        case TokenType::DB_UPDATE:
            return parse_update_statement();
        case TokenType::DB_DELETE:
            return parse_delete_statement();
        case TokenType::DB_CREATE:
            return parse_create_statement();
        case TokenType::DB_DROP:
            return parse_drop_statement();
        case TokenType::DB_USE:
            return parse_use_statement();
        case TokenType::DB_ALTER:
            return parse_alter_statement();
        case TokenType::DB_SHOW:
            return parse_show_statement();
        case TokenType::DB_DESCRIBE:
        case TokenType::DB_DESC:
            return parse_describe_statement();
        default:
            error("Unexpected token");
            return nullptr;
    }
}

std::unique_ptr<AstStatementNode> Parser::parse_select_statement()
{
    // 获取 SELECT 关键字的位置信息
    std::size_t line = current_token_.get_line();
    std::size_t column = current_token_.get_column();

    // 创建 Select 语句节点
    auto select_statement = std::make_unique<AstSelectStatementNode>(line, column);

    // 消耗 SELECT 关键字
    advance();

    // 解析选择项列表
    do {
        if (match(TokenType::DB_STAR)) {
            // 添加 * 列名
            select_statement->add_select_item(SelectItem::create_star_item());
        } else {
            auto expr = parse_expression();
            // 添加选择项
            select_statement->add_select_item(SelectItem::create_expression_item(std::move(expr)));
        }
    } while (match(TokenType::DB_COMMA));

    // 期望 FROM 关键字
    consume(TokenType::DB_FROM, "Expected FROM after select items");

    // 解析集合名称
    if (!check(TokenType::DB_IDENTIFIER)) {
        error("Expected collection_name after FROM");
        return nullptr;
    }
    std::string collection_name = current_token_.get_value();
    // 消耗标识符
    advance();
    // 设置集合名称
    select_statement->set_collection_name(collection_name);

    // 解析可选的 WHERE 子句
    if (match(TokenType::DB_WHERE)) {
        auto where_clause = parse_expression();
        // 设置 WHERE 子句
        select_statement->set_where_clause(std::move(where_clause));
    }

    // 解析可选的 GROUP BY 子句
    if (match(TokenType::DB_GROUP)) {
        // 期望 BY 关键字
        consume(TokenType::DB_BY, "Expected BY after GROUP");

        // 解析 GROUP BY 子句
        do {
            auto group_by_clause = parse_expression();
            // 添加 GROUP BY 子句
            select_statement->add_group_by_clause(std::move(group_by_clause));
        } while (match(TokenType::DB_COMMA));
    }

    // 解析可选的 HAVING 子句
    if (match(TokenType::DB_HAVING)) {
        auto having_clause = parse_expression();
        // 设置 HAVING 子句
        select_statement->set_having_clause(std::move(having_clause));
    }

    // 解析可选的 ORDER BY 子句
    if (match(TokenType::DB_ORDER)) {
        // 期望 BY 关键字
        consume(TokenType::DB_BY, "Expected BY after ORDER");

        // 解析 ORDER BY 子句
        do {
            auto order_by_clause = parse_expression();
            
            // 解析可选的方向
            Direction order_type = Direction::ASC;
            if (match(TokenType::DB_ASC)) {
                order_type = Direction::ASC;
            } else if (match(TokenType::DB_DESC)) {
                order_type = Direction::DESC;
            }

            // 创建 OrderByItem 对象
            OrderByItem order_by_item(std::move(order_by_clause), order_type);
            // 添加 OrderByItem 对象
            select_statement->add_order_by_item(std::move(order_by_item));
        } while (match(TokenType::DB_COMMA));
    }

    // 解析可选的 LIMIT 子句
    if (match(TokenType::DB_LIMIT)) {
        auto limit_clause = parse_expression();
        // 设置 LIMIT 子句
        select_statement->set_limit(std::move(limit_clause));
    }

    // 解析可选的 OFFSET 子句
    if (match(TokenType::DB_OFFSET)) {
        auto offset_clause = parse_expression();
        // 设置 OFFSET 子句
        select_statement->set_offset(std::move(offset_clause));
    }

    // SELECT 语句解析完成
    return select_statement;
}

std::unique_ptr<AstStatementNode> Parser::parse_insert_statement()
{
    // 获取 INSERT 关键字的位置信息
    std::size_t line = current_token_.get_line();
    std::size_t column = current_token_.get_column();

    // 创建 Insert 语句节点
    auto insert_statement = std::make_unique<AstInsertStatementNode>(line, column);

    // 消耗 INSERT 关键字
    advance();

    // 期望 INTO 关键字
    consume(TokenType::DB_INTO, "Expected INTO after INSERT");

    // 解析集合名称
    if (!check(TokenType::DB_IDENTIFIER)) {
        error("Expected collection_name after INTO");
        return nullptr;
    }
    std::string collection_name = current_token_.get_value();
    // 消耗标识符
    advance();
    // 设置集合名称
    insert_statement->set_collection_name(collection_name);

    // 检查是否存在列名列表
    if (match(TokenType::DB_LEFT_PAREN)) {
        // 解析列名列表
        do {
            if (!check(TokenType::DB_IDENTIFIER)) {
                error("Expected column_name after '('");
                return nullptr;
            }
            std::string column_name = current_token_.get_value();
            // 消耗标识符
            advance();
            // 添加列名
            insert_statement->add_column_name(column_name);
        } while (match(TokenType::DB_COMMA));

        // 期望 )
        consume(TokenType::DB_RIGHT_PAREN, "Expected ')' after column_names");
    }

    // 期望 VALUES 关键字
    consume(TokenType::DB_VALUES, "Expected VALUES after INTO");

    // 期望 (
    consume(TokenType::DB_LEFT_PAREN, "Expected '(' after VALUES");

    // 解析值列表
    do {
        auto value = parse_expression();
        // 添加值
        insert_statement->add_value(std::move(value));
    } while (match(TokenType::DB_COMMA));

    // 期望 )
    consume(TokenType::DB_RIGHT_PAREN, "Expected ')' after values");

    // INSERT 语句解析完成
    return insert_statement;
}

std::unique_ptr<AstStatementNode> Parser::parse_update_statement()
{
    // 获取 UPDATE 关键字的位置信息
    std::size_t line = current_token_.get_line();
    std::size_t column = current_token_.get_column();

    // 创建 Update 语句节点
    auto update_statement = std::make_unique<AstUpdateStatementNode>(line, column);

    // 消耗 UPDATE 关键字
    advance();

    // 解析集合名称
    if (!check(TokenType::DB_IDENTIFIER)) {
        error("Expected collection_name after UPDATE");
        return nullptr;
    }
    std::string collection_name = current_token_.get_value();
    // 消耗标识符
    advance();
    // 设置集合名称
    update_statement->set_collection_name(collection_name);

    // 期望 SET 关键字
    consume(TokenType::DB_SET, "Expected SET after UPDATE");

    // 解析赋值项列表
    do {
        if (!check(TokenType::DB_IDENTIFIER)) {
            error("Expected column_name after SET");
            return nullptr;
        }
        std::string column_name = current_token_.get_value();
        // 消耗标识符
        advance();
        
        // 期望 =
        consume(TokenType::DB_EQUAL, "Expected = after column_name");

        // 解析值
        auto value = parse_expression();

        // 创建 UpdateAssignment 对象
        UpdateAssignment assignment(column_name, std::move(value));
        // 添加 UpdateAssignment 对象
        update_statement->add_assignment(std::move(assignment));
    } while (match(TokenType::DB_COMMA));

    // 解析可选的 WHERE 子句
    if (match(TokenType::DB_WHERE)) {
        auto where_clause = parse_expression();
        // 设置 WHERE 子句
        update_statement->set_where_clause(std::move(where_clause));
    }

    // UPDATE 语句解析完成
    return update_statement;
}

std::unique_ptr<AstStatementNode> Parser::parse_delete_statement()
{
    // 获取 DELETE 关键字的位置信息
    std::size_t line = current_token_.get_line();
    std::size_t column = current_token_.get_column();

    // 创建 Delete 语句节点
    auto delete_statement = std::make_unique<AstDeleteStatementNode>(line, column);

    // 消耗 DELETE 关键字
    advance();

    // 期望 FROM 关键字
    consume(TokenType::DB_FROM, "Expected FROM after DELETE");

    // 解析集合名称
    if (!check(TokenType::DB_IDENTIFIER)) {
        error("Expected collection_name after DELETE");
        return nullptr;
    }
    std::string collection_name = current_token_.get_value();
    // 消耗标识符
    advance();
    // 设置集合名称
    delete_statement->set_collection_name(collection_name);

    // 解析可选的 WHERE 子句
    if (match(TokenType::DB_WHERE)) {
        auto where_clause = parse_expression();
        // 设置 WHERE 子句
        delete_statement->set_where_clause(std::move(where_clause));
    }

    // DELETE 语句解析完成
    return delete_statement;
}

std::unique_ptr<AstStatementNode> Parser::parse_create_statement()
{
    // 获取 CREATE 关键字的位置信息
    std::size_t line = current_token_.get_line();
    std::size_t column = current_token_.get_column();

    // 创建 Create 语句节点
    auto create_statement = std::make_unique<AstCreateStatementNode>(line, column);

    // 消耗 CREATE 关键字
    advance();

    // 解析对象类型
    if (match(TokenType::DB_DATABASE)) {
        // 解析是否需要进行存在性检查
        if (match(TokenType::DB_IF)) {
            consume(TokenType::DB_EXISTS, "Expected EXISTS after IF");
            create_statement->set_if_not_exists(true);
        }

        // 解析数据库名称
        if (!check(TokenType::DB_IDENTIFIER)) {
            error("Expected database_name after DATABASE");
            return nullptr;
        }
        std::string database_name = current_token_.get_value();
        // 消耗标识符
        advance();

        // 创建 AstCreateDatabase 操作
        AstCreateDatabase create_database(database_name);

        // 设置 AstCreateDatabase 操作
        create_statement->set_create_database(std::move(create_database));
    } else if (match(TokenType::DB_COLLECTION)) {
        // 解析是否需要进行存在性检查
        if (match(TokenType::DB_IF)) {
            consume(TokenType::DB_EXISTS, "Expected EXISTS after IF");
            create_statement->set_if_not_exists(true);
        }

        // 解析集合名称
        if (!check(TokenType::DB_IDENTIFIER)) {
            error("Expected collection_name after COLLECTION");
            return nullptr;
        }
        std::string collection_name = current_token_.get_value();
        // 消耗标识符
        advance();

        // 期望 (
        consume(TokenType::DB_LEFT_PAREN, "Expected '(' after collection_name");

        // 解析列定义列表
        std::vector<AstColumnDefinition> column_definitions;
        do {
            auto column_definition = parse_column_definition();

            // 添加列定义
            column_definitions.push_back(std::move(column_definition));
        } while (match(TokenType::DB_COMMA));

        // 期望 )
        consume(TokenType::DB_RIGHT_PAREN, "Expected ')' after column_definitions");

        // 创建 AstCreateCollection 操作
        AstCreateCollection create_collection(collection_name, std::move(column_definitions));

        // 设置 AstCreateCollection 操作
        create_statement->set_create_collection(std::move(create_collection));
    } else if (match(TokenType::DB_INDEX)) {
        // 解析是否需要进行存在性检查
        if (match(TokenType::DB_IF)) {
            consume(TokenType::DB_EXISTS, "Expected EXISTS after IF");
            create_statement->set_if_not_exists(true);
        }

        // 解析索引名称
        if (!check(TokenType::DB_IDENTIFIER)) {
            error("Expected index_name after INDEX");
            return nullptr;
        }
        std::string index_name = current_token_.get_value();
        // 消耗标识符
        advance();

        // 期望 ON 关键字
        consume(TokenType::DB_ON, "Expected ON after INDEX");

        // 解析集合名称
        if (!check(TokenType::DB_IDENTIFIER)) {
            error("Expected collection_name after ON");
            return nullptr;
        }
        std::string collection_name = current_token_.get_value();
        // 消耗标识符
        advance();

        // 期望 (
        consume(TokenType::DB_LEFT_PAREN, "Expected '(' after ON");

        // 解析列名列表
        std::vector<std::string> column_names;
        do {
            if (!check(TokenType::DB_IDENTIFIER)) {
                error("Expected column_name after '('");
                return nullptr;
            }
            std::string column_name = current_token_.get_value();
            // 消耗标识符
            advance();
            column_names.push_back(column_name);
        } while (match(TokenType::DB_COMMA));

        // 期望 )
        consume(TokenType::DB_RIGHT_PAREN, "Expected ')' after column_names");

        std::string index_type;
        // 检查是否存在 USING
        if (match(TokenType::DB_USING)) {
            // 解析索引类型
            if (!check(TokenType::DB_IDENTIFIER)) {
                error("Expected index_type after USING");
                return nullptr;
            }
            index_type = current_token_.get_value();
            // 消耗标识符
            advance();
        }

        // 创建 AstCreateIndex 操作
        AstCreateIndex create_index(index_name, collection_name, column_names, index_type);

        // 设置 AstCreateIndex 操作
        create_statement->set_create_index(std::move(create_index));
    } else if (match(TokenType::DB_VINDEX)) {
        // 解析是否需要进行存在性检查
        if (match(TokenType::DB_IF)) {
            consume(TokenType::DB_EXISTS, "Expected EXISTS after IF");
            create_statement->set_if_not_exists(true);
        }

        // 解析向量索引名称
        if (!check(TokenType::DB_IDENTIFIER)) {
            error("Expected vindex_name after VINDEX");
            return nullptr;
        }
        std::string vindex_name = current_token_.get_value();
        // 消耗标识符
        advance();

        // 期望 ON 关键字
        consume(TokenType::DB_ON, "Expected ON after VINDEX");

        // 解析集合名称
        if (!check(TokenType::DB_IDENTIFIER)) {
            error("Expected collection_name after ON");
            return nullptr;
        }
        std::string collection_name = current_token_.get_value();
        // 消耗标识符
        advance();

        // 期望 (
        consume(TokenType::DB_LEFT_PAREN, "Expected '(' after ON");

        // 解析列名
        if (!check(TokenType::DB_IDENTIFIER)) {
            error("Expected column_name after '('");
            return nullptr;
        }
        std::string column_name = current_token_.get_value();
        // 消耗标识符
        advance();

        // 期望 )
        consume(TokenType::DB_RIGHT_PAREN, "Expected ')' after column_name");

        // 查看是否存在 USING
        std::string vindex_type;
        std::vector<AstVIndexWithOption> with_clauses;
        if (match(TokenType::DB_USING)) {
            // 解析向量索引类型
            if (!check(TokenType::DB_IDENTIFIER)) {
                error("Expected vindex_type after USING");
                return nullptr;
            }
            vindex_type = current_token_.get_value();
            // 消耗标识符
            advance();

            // 查看是否存在 WITH
            if (match(TokenType::DB_WITH)) {
                // 期望 (
                consume(TokenType::DB_LEFT_PAREN, "Expected '(' after WITH");

                // 解析 WITH 子句
                do {
                    if (!check(TokenType::DB_IDENTIFIER)) {
                        error("Expected key after WITH");
                        return nullptr;
                    }
                    std::string key = current_token_.get_value();
                    // 消耗标识符
                    advance();

                    // 期望 =
                    consume(TokenType::DB_EQUAL, "Expected = after key");

                    // 解析值
                    auto value = parse_expression();
                    // 添加 WITH 子句
                    with_clauses.push_back(AstVIndexWithOption(key, std::move(value)));
                } while (match(TokenType::DB_COMMA));

                // 期望 )
                consume(TokenType::DB_RIGHT_PAREN, "Expected ')' after WITH");
            }
        }

        // 创建 AstCreateVIndex 操作
        AstCreateVIndex create_vindex(vindex_name, collection_name, column_name, vindex_type, std::move(with_clauses));

        // 设置 AstCreateVIndex 操作
        create_statement->set_create_vindex(std::move(create_vindex));
    } else {
        error("Expected DATABASE, COLLECTION, INDEX or VINDEX after CREATE");
        return nullptr;
    }

    // CREATE 语句解析完成
    return create_statement;
}

std::unique_ptr<AstStatementNode> Parser::parse_drop_statement()
{
    // 获取 DROP 关键字的位置信息
    std::size_t line = current_token_.get_line();
    std::size_t column = current_token_.get_column();

    // 创建 Drop 语句节点
    auto drop_statement = std::make_unique<AstDropStatementNode>(line, column);

    // 消耗 DROP 关键字
    advance();

    // 解析对象类型
    if (match(TokenType::DB_DATABASE)) {
        // 解析是否需要进行存在性检查
        if (match(TokenType::DB_IF)) {
            consume(TokenType::DB_EXISTS, "Expected EXISTS after IF");
            drop_statement->set_if_exists(true);
        }

        // 解析数据库名称
        if (!check(TokenType::DB_IDENTIFIER)) {
            error("Expected database_name after DATABASE");
            return nullptr;
        }
        std::string database_name = current_token_.get_value();
        // 消耗标识符
        advance();

        // 创建 AstDropDatabase 操作
        AstDropDatabase drop_database(database_name);

        // 设置 AstDropDatabase 操作
        drop_statement->set_drop_database(std::move(drop_database));
    } else if (match(TokenType::DB_COLLECTION)) {
        // 解析是否需要进行存在性检查
        if (match(TokenType::DB_IF)) {
            consume(TokenType::DB_EXISTS, "Expected EXISTS after IF");
            drop_statement->set_if_exists(true);
        }

        // 解析集合名称
        if (!check(TokenType::DB_IDENTIFIER)) {
            error("Expected collection_name after COLLECTION");
            return nullptr;
        }
        std::string collection_name = current_token_.get_value();
        // 消耗标识符
        advance();

        // 创建 AstDropCollection 操作
        AstDropCollection drop_collection(collection_name);

        // 设置 AstDropCollection 操作
        drop_statement->set_drop_collection(std::move(drop_collection));
    } else if (match(TokenType::DB_INDEX)) {
        // 解析是否需要进行存在性检查
        if (match(TokenType::DB_IF)) {
            consume(TokenType::DB_EXISTS, "Expected EXISTS after IF");
            drop_statement->set_if_exists(true);
        }

        // 解析索引名称
        if (!check(TokenType::DB_IDENTIFIER)) {
            error("Expected index_name after INDEX");
            return nullptr;
        }
        std::string index_name = current_token_.get_value();
        // 消耗标识符
        advance();

        // 期望 FROM 关键字
        consume(TokenType::DB_FROM, "Expected FROM after INDEX");

        // 解析集合名称
        if (!check(TokenType::DB_IDENTIFIER)) {
            error("Expected collection_name after FROM");
            return nullptr;
        }
        std::string collection_name = current_token_.get_value();
        // 消耗标识符
        advance();

        // 创建 AstDropIndex 操作
        AstDropIndex drop_index(index_name, collection_name);

        // 设置 AstDropIndex 操作
        drop_statement->set_drop_index(std::move(drop_index));
    } else if (match(TokenType::DB_VINDEX)) {
        // 解析是否需要进行存在性检查
        if (match(TokenType::DB_IF)) {
            consume(TokenType::DB_EXISTS, "Expected EXISTS after IF");
            drop_statement->set_if_exists(true);
        }

        // 解析向量索引名称
        if (!check(TokenType::DB_IDENTIFIER)) {
            error("Expected vindex_name after VINDEX");
            return nullptr;
        }
        std::string vindex_name = current_token_.get_value();
        // 消耗标识符
        advance();

        // 期望 FROM 关键字
        consume(TokenType::DB_FROM, "Expected FROM after VINDEX");

        // 解析集合名称
        if (!check(TokenType::DB_IDENTIFIER)) {
            error("Expected collection_name after FROM");
            return nullptr;
        }
        std::string collection_name = current_token_.get_value();
        // 消耗标识符
        advance();

        // 创建 AstDropVIndex 操作
        AstDropVIndex drop_vindex(vindex_name, collection_name);

        // 设置 AstDropVIndex 操作
        drop_statement->set_drop_vindex(std::move(drop_vindex));
    } else {
        error("Expected DATABASE, COLLECTION, INDEX or VINDEX after DROP");
        return nullptr;
    }

    // DROP 语句解析完成
    return drop_statement;
}

std::unique_ptr<AstStatementNode> Parser::parse_use_statement()
{
    // 获取 USE 关键字的位置信息
    std::size_t line = current_token_.get_line();
    std::size_t column = current_token_.get_column();

    // 创建 Use 语句节点
    auto use_statement = std::make_unique<AstUseStatementNode>(line, column);

    // 消耗 USE 关键字
    advance();

    // 解析数据库名称
    if (!check(TokenType::DB_IDENTIFIER)) {
        error("Expected database_name after USE");
        return nullptr;
    }
    std::string database_name = current_token_.get_value();
    // 消耗标识符
    advance();

    // 设置数据库名称
    use_statement->set_database_name(database_name);

    // USE 语句解析完成
    return use_statement;
}

std::unique_ptr<AstStatementNode> Parser::parse_alter_statement()
{
    // 获取 ALTER 关键字的位置信息
    std::size_t line = current_token_.get_line();
    std::size_t column = current_token_.get_column();

    // 创建 Alter 语句节点
    auto alter_statement = std::make_unique<AstAlterStatementNode>(line, column);

    // 消耗 ALTER 关键字
    advance();

    // 期望 COLLECTION 关键字
    consume(TokenType::DB_COLLECTION, "Expected COLLECTION after ALTER");

    // 解析集合名称
    if (!check(TokenType::DB_IDENTIFIER)) {
        error("Expected collection_name after COLLECTION");
        return nullptr;
    }
    std::string collection_name = current_token_.get_value();
    // 消耗标识符
    advance();

    // 解析对象类型
    if (match(TokenType::DB_ADD)) {
        // 期望 COLUMN 关键字
        consume(TokenType::DB_COLUMN, "Expected COLUMN after ADD");

        // 解析列定义
        auto column_definition = parse_column_definition();

        // 创建 AstAlterAddColumn 操作
        AstAlterAddColumn add_column(std::move(column_definition));

        // 设置 AstAlterAddColumn 操作
        alter_statement->set_add_column(std::move(add_column));
    } else if (match(TokenType::DB_DROP)) {
        // 期望 COLUMN 关键字
        consume(TokenType::DB_COLUMN, "Expected COLUMN after DROP");

        // 解析字段名称
        if (!check(TokenType::DB_IDENTIFIER)) {
            error("Expected column_name after COLUMN");
            return nullptr;
        }
        std::string column_name = current_token_.get_value();
        // 消耗标识符
        advance();

        // 创建 AstAlterDropColumn 操作
        AstAlterDropColumn drop_column(column_name);

        // 设置 AstAlterDropColumn 操作
        alter_statement->set_drop_column(std::move(drop_column));
    } else if (match(TokenType::DB_MODIFY)) {
        // 期望 COLUMN 关键字
        consume(TokenType::DB_COLUMN, "Expected COLUMN after MODIFY");

        // 解析列定义
        auto column_definition = parse_column_definition();

        // 创建 AstAlterModifyColumn 操作
        AstAlterModifyColumn modify_column(std::move(column_definition));

        // 设置 AstAlterModifyColumn 操作
        alter_statement->set_modify_column(std::move(modify_column));
    } else if (match(TokenType::DB_RENAME)) {
        // 期望 COLUMN 关键字
        consume(TokenType::DB_COLUMN, "Expected COLUMN after RENAME");

        // 解析字段名称
        if (!check(TokenType::DB_IDENTIFIER)) {
            error("Expected column_name after COLUMN");
            return nullptr;
        }
        std::string old_column_name = current_token_.get_value();
        // 消耗标识符
        advance();

        // 期望 TO 关键字
        consume(TokenType::DB_TO, "Expected TO after COLUMN");

        // 解析新字段名称
        if (!check(TokenType::DB_IDENTIFIER)) {
            error("Expected new_column_name after TO");
            return nullptr;
        }
        std::string new_column_name = current_token_.get_value();
        // 消耗标识符
        advance();

        // 创建 AstAlterRenameColumn 操作
        AstAlterRenameColumn rename_column(old_column_name, new_column_name);

        // 设置 AstAlterRenameColumn 操作
        alter_statement->set_rename_column(std::move(rename_column));
    } else {
        error("Expected ADD, DROP, MODIFY or RENAME after ALTER");
        return nullptr;
    }

    // ALTER 语句解析完成
    return alter_statement;
}

std::unique_ptr<AstStatementNode> Parser::parse_show_statement()
{
    // 获取 SHOW 关键字的位置信息
    std::size_t line = current_token_.get_line();
    std::size_t column = current_token_.get_column();

    // 创建 Show 语句节点
    auto show_statement = std::make_unique<AstShowStatementNode>(line, column);

    // 消耗 SHOW 关键字
    advance();

    // 解析对象类型
    if (match(TokenType::DB_DATABASES)) {
        // 创建 AstShowDatabases 操作
        AstShowDatabases show_databases;

        // 设置 AstShowDatabases 操作
        show_statement->set_show_databases(std::move(show_databases));
    } else if (match(TokenType::DB_COLLECTIONS)) {
        // 创建 AstShowCollections 操作
        AstShowCollections show_collections;

        // 判断是否指定数据库
        if (match(TokenType::DB_FROM)) {
            // 解析数据库名称
            if (!check(TokenType::DB_IDENTIFIER)) {
                error("Expected database_name after FROM");
                return nullptr;
            }
            std::string database_name = current_token_.get_value();
            // 消耗标识符
            advance();

            // 设置数据库名称
            show_collections.set_database_name(database_name);
        }

        // 设置 AstShowCollections 操作
        show_statement->set_show_collections(std::move(show_collections));
    } else if (match(TokenType::DB_INDEXES)) {
        // 期望 FROM 关键字
        consume(TokenType::DB_FROM, "Expected FROM after INDEXES");

        // 解析集合名称
        if (!check(TokenType::DB_IDENTIFIER)) {
            error("Expected collection_name after FROM");
            return nullptr;
        }
        std::string collection_name = current_token_.get_value();
        // 消耗标识符
        advance();

        // 创建 AstShowIndexes 操作
        AstShowIndexes show_indexes(collection_name);

        // 判断是否指定数据库
        if (match(TokenType::DB_FROM)) {
            // 解析数据库名称
            if (!check(TokenType::DB_IDENTIFIER)) {
                error("Expected database_name after FROM");
                return nullptr;
            }
            std::string database_name = current_token_.get_value();
            advance();

            // 设置数据库名称
            show_indexes.set_database_name(database_name);
        }

        // 设置 AstShowIndexes 操作
        show_statement->set_show_indexes(std::move(show_indexes));
    } else if (match(TokenType::DB_VINDEXES)) {
        // 期望 FROM 关键字
        consume(TokenType::DB_FROM, "Expected FROM after VINDEXES");

        // 解析集合名称
        if (!check(TokenType::DB_IDENTIFIER)) {
            error("Expected collection_name after FROM");
            return nullptr;
        }
        std::string collection_name = current_token_.get_value();
        // 消耗标识符
        advance();

        // 创建 AstShowVIndexes 操作
        AstShowVIndexes show_vindexes(collection_name);

        // 判断是否指定数据库
        if (match(TokenType::DB_FROM)) {
            // 解析数据库名称
            if (!check(TokenType::DB_IDENTIFIER)) {
                error("Expected database_name after FROM");
                return nullptr;
            }
            std::string database_name = current_token_.get_value();
            advance();

            // 设置数据库名称
            show_vindexes.set_database_name(database_name);
        }

        // 设置 AstShowVIndexes 操作
        show_statement->set_show_vindexes(std::move(show_vindexes));
    } else {
        error("Expected DATABASES, COLLECTIONS, INDEXES or VINDEXES after SHOW");
        return nullptr;
    }

    // SHOW 语句解析完成
    return show_statement;
}

std::unique_ptr<AstStatementNode> Parser::parse_describe_statement()
{
    // 获取 DESCRIBE 关键字的位置信息
    std::size_t line = current_token_.get_line();
    std::size_t column = current_token_.get_column();

    // 创建 Describe 语句节点
    auto describe_statement = std::make_unique<AstDescribeStatementNode>(line, column);

    // 消耗 DESCRIBE 关键字
    advance();

    // 解析集合名称
    if (!check(TokenType::DB_IDENTIFIER)) {
        error("Expected collection_name after DESCRIBE");
        return nullptr;
    }
    std::string collection_name = current_token_.get_value();
    // 消耗标识符
    advance();

    // 设置集合名称
    describe_statement->set_collection_name(collection_name);

    // DESCRIBE 语句解析完成
    return describe_statement;
}

std::unique_ptr<AstExpressionNode> Parser::parse_expression()
{
    // 该接口是表达式解析入口，具体解析逻辑在各个解析函数中实现
    // 递归下降解析器会自动处理所有优先级

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

    // 能够进入递归下降解析表达式的 Token 类型
    switch (current_token_.get_type()) {
        // 字面量：数字、字符串、布尔值、NULL
        case TokenType::DB_INTEGER_LITERAL:
        case TokenType::DB_FLOAT_LITERAL:
        case TokenType::DB_STRING_LITERAL:
        case TokenType::DB_TRUE:
        case TokenType::DB_FALSE:
        case TokenType::DB_NULL:
        // 标识符：字段名、变量等
        case TokenType::DB_IDENTIFIER:
        // 一元运算符：+、-、NOT
        case TokenType::DB_PLUS:
        case TokenType::DB_MINUS:
        case TokenType::DB_NOT:
        // 括号表达式 '('
        case TokenType::DB_LEFT_PAREN:
        // 向量字面量 '['
        case TokenType::DB_LEFT_BRACKET:
            // 调用最低优先级解析函数
            return parse_or_expression();
        default: {
            error("Expected expression");
            return nullptr;
        }
    }
}

std::unique_ptr<AstExpressionNode> Parser::parse_or_expression()
{
    // 递归下降解析左侧的 AND 表达式
    auto left = parse_and_expression();
    if (left == nullptr) {
        error("Expected expression before OR");
        return nullptr;
    }

    // 左结合循环处理多个 OR 运算符
    // 例如: a OR b OR c 解析为 ((a OR b) OR c)
    while (check(TokenType::DB_OR)) {
        // 保存 OR token 的位置信息
        std::size_t line = current_token_.get_line();
        std::size_t column = current_token_.get_column();

        // 消耗 OR 关键字
        advance();

        // 创建二元表达式节点
        auto expr = std::make_unique<AstBinaryExpressionNode>(line, column);

        // 设置运算符类型为 OR
        expr->set_operator_type(AstBinaryOperatorType::AST_BINARY_OPERATOR_OR);

        // 设置左操作数为之前解析的结果
        expr->set_left(std::move(left));

        // 递归下降解析右侧的 AND 表达式
        auto right = parse_and_expression();
        if (right == nullptr) {
            error("Expected expression after OR");
            return nullptr;
        }
        expr->set_right(std::move(right));

        // 将当前表达式作为下一次的左操作数
        left = std::move(expr);
    }

    // OR 表达式解析完成
    return left;
}

std::unique_ptr<AstExpressionNode> Parser::parse_and_expression()
{
    // 递归下降解析左侧的比较表达式
    auto left = parse_comparison_expression();
    if (left == nullptr) {
        error("Expected expression before AND");
        return nullptr;
    }

    // 左结合循环处理多个 AND 运算符
    // 例如: a AND b AND c 解析为 ((a AND b) AND c)
    while (check(TokenType::DB_AND)) {
        // 保存 AND token 的位置信息
        std::size_t line = current_token_.get_line();
        std::size_t column = current_token_.get_column();

        // 消耗 AND 关键字
        advance();

        // 创建二元表达式节点
        auto expr = std::make_unique<AstBinaryExpressionNode>(line, column);

        // 设置运算符类型为 AND
        expr->set_operator_type(AstBinaryOperatorType::AST_BINARY_OPERATOR_AND);

        // 设置左操作数为之前解析的结果
        expr->set_left(std::move(left));

        // 递归下降解析右侧的比较表达式
        auto right = parse_comparison_expression();
        if (right == nullptr) {
            error("Expected expression after AND");
            return nullptr;
        }
        expr->set_right(std::move(right));

        // 将当前表达式作为下一次的左操作数
        left = std::move(expr);
    }

    // AND 表达式解析完成
    return left;
}

std::unique_ptr<AstExpressionNode> Parser::parse_comparison_expression()
{
    // 递归下降解析左侧的加法表达式
    auto left = parse_additive_expression();
    if (left == nullptr) {
        error("Expected expression before comparison");
        return nullptr;
    }

    // 判断是否有 NOT 关键字
    // NOT IN, NOT BETWEEN, NOT LIKE 会使用该变量
    bool is_not = match(TokenType::DB_NOT);

    std::size_t line = current_token_.get_line();
    std::size_t column = current_token_.get_column();

    // 根据当前 Token 类型进行不同的比较表达式解析
    switch (current_token_.get_type()) {
        case TokenType::DB_EQUAL:
        case TokenType::DB_NOT_EQUAL:
        case TokenType::DB_LESS_THAN:
        case TokenType::DB_GREATER_THAN:
        case TokenType::DB_LESS_EQUAL:
        case TokenType::DB_GREATER_EQUAL: {
            if (is_not) {
                error("Unexpected NOT before comparison");
                return nullptr;
            }

            AstBinaryOperatorType operator_type;
            switch (current_token_.get_type()) {
                case TokenType::DB_EQUAL:
                    operator_type = AstBinaryOperatorType::AST_BINARY_OPERATOR_EQUAL;
                    break;
                case TokenType::DB_NOT_EQUAL:
                    operator_type = AstBinaryOperatorType::AST_BINARY_OPERATOR_NOT_EQUAL;
                    break;
                case TokenType::DB_LESS_THAN:
                    operator_type = AstBinaryOperatorType::AST_BINARY_OPERATOR_LESS_THAN;
                    break;
                case TokenType::DB_GREATER_THAN:
                    operator_type = AstBinaryOperatorType::AST_BINARY_OPERATOR_GREATER_THAN;
                    break;
                case TokenType::DB_LESS_EQUAL:
                    operator_type = AstBinaryOperatorType::AST_BINARY_OPERATOR_LESS_EQUAL;
                    break;
                case TokenType::DB_GREATER_EQUAL:
                    operator_type = AstBinaryOperatorType::AST_BINARY_OPERATOR_GREATER_EQUAL;
                    break;
                default:
                    break;
            }

            // 消耗比较运算符
            advance();

            // 递归解析右侧的加法表达式
            auto right = parse_additive_expression();
            if (right == nullptr) {
                error("Expected expression after comparison");
                return nullptr;
            }
            
            // 创建二元表达式节点
            auto binary_expression = std::make_unique<AstBinaryExpressionNode>(line, column);
            binary_expression->set_operator_type(operator_type);
            binary_expression->set_left(std::move(left));
            binary_expression->set_right(std::move(right));
            return binary_expression;
        }
        case TokenType::DB_IN:
            return parse_in_expression(std::move(left), is_not);
        case TokenType::DB_BETWEEN:
            return parse_between_expression(std::move(left), is_not);
        case TokenType::DB_LIKE:
            return parse_like_expression(std::move(left), is_not);
        default:
            // 如果前面有 NOT，但后面不是特殊运算符，包成 UnaryExpression
            if (is_not) {
                auto unary_expression = std::make_unique<AstUnaryExpressionNode>(line, column);
                unary_expression->set_operator_type(AstUnaryOperatorType::AST_UNARY_OPERATOR_NOT);
                unary_expression->set_operand(std::move(left));
                return unary_expression;
            } else {
                return left;
            }
    }
}

std::unique_ptr<AstExpressionNode> Parser::parse_additive_expression()
{
    // 递归下降解析左侧的乘法表达式
    auto left = parse_multiplicative_expression();
    if (left == nullptr) {
        error("Expected expression before additive");
        return nullptr;
    }

    // 左结合循环处理多个加法运算符
    // 例如: a + b + c 解析为 ((a + b) + c)
    while (check(TokenType::DB_PLUS) || check(TokenType::DB_MINUS)) {
        // 保存加号或减号 Token 的位置信息
        std::size_t line = current_token_.get_line();
        std::size_t column = current_token_.get_column();
        // 获取加号或减号 Token 类型
        TokenType operator_type = current_token_.get_type();

        // 消耗加号或减号关键字
        advance();

        // 创建二元表达式节点
        auto binary_expression = std::make_unique<AstBinaryExpressionNode>(line, column);
        // 设置运算符类型为加号或减号
        if (operator_type == TokenType::DB_PLUS) {
            binary_expression->set_operator_type(AstBinaryOperatorType::AST_BINARY_OPERATOR_PLUS);
        } else if (operator_type == TokenType::DB_MINUS) {
            binary_expression->set_operator_type(AstBinaryOperatorType::AST_BINARY_OPERATOR_MINUS);
        } else {
            error("Unexpected operator in additive expression");
            return nullptr;
        }

        // 设置左操作数为之前解析的结果
        binary_expression->set_left(std::move(left));

        // 递归解析右侧的乘法表达式
        auto right = parse_multiplicative_expression();
        if (right == nullptr) {
            error("Expected expression after additive");
            return nullptr;
        }
        // 设置右操作数为右侧解析的结果
        binary_expression->set_right(std::move(right));

        // 将当前表达式作为下一次的左操作数
        left = std::move(binary_expression);
    }

    // 加法表达式解析完成
    return left;
}

std::unique_ptr<AstExpressionNode> Parser::parse_multiplicative_expression()
{
    // 递归下降解析左侧的一元表达式
    auto left = parse_unary_expression();
    if (left == nullptr) {
        error("Expected expression before multiplicative");
        return nullptr;
    }

    // 左结合循环处理多个乘法运算符
    // 例如: a * b * c 解析为 ((a * b) * c)
    // 注：Token 类型命名为 STAR 和 SLASH 是为了描述 * 和 / 的形态，而不是数学语义
    // 而 AstBinaryOperatorType 中使用 MULTIPLY 和 DIVIDE 是为了描述 * 和 / 的数学含义
    while (check(TokenType::DB_STAR) || check(TokenType::DB_SLASH) || check(TokenType::DB_MODULO)) {
        // 保存乘号、除号或取模号 Token 的位置信息
        std::size_t line = current_token_.get_line();
        std::size_t column = current_token_.get_column();
        // 获取乘号、除号或取模号 Token 类型
        TokenType operator_type = current_token_.get_type();

        // 消耗乘号、除号或取模号关键字
        advance();

        // 创建二元表达式节点
        auto binary_expression = std::make_unique<AstBinaryExpressionNode>(line, column);
        // 设置运算符类型为乘号、除号或取模号
        if (operator_type == TokenType::DB_STAR) {
            binary_expression->set_operator_type(AstBinaryOperatorType::AST_BINARY_OPERATOR_MULTIPLY);
        } else if (operator_type == TokenType::DB_SLASH) {
            binary_expression->set_operator_type(AstBinaryOperatorType::AST_BINARY_OPERATOR_DIVIDE);
        } else if (operator_type == TokenType::DB_MODULO) {
            binary_expression->set_operator_type(AstBinaryOperatorType::AST_BINARY_OPERATOR_MODULO);
        } else {
            error("Unexpected operator in multiplicative expression");
            return nullptr;
        }

        // 设置左操作数为之前解析的结果
        binary_expression->set_left(std::move(left));

        // 递归解析右侧的一元表达式
        auto right = parse_unary_expression();
        if (right == nullptr) {
            error("Expected expression after multiplicative");
            return nullptr;
        }
        // 设置右操作数为右侧解析的结果
        binary_expression->set_right(std::move(right));

        // 将当前表达式作为下一次的左操作数
        left = std::move(binary_expression);
    }

    // 乘法表达式解析完成
    return left;
}

std::unique_ptr<AstExpressionNode> Parser::parse_unary_expression()
{
    // 解析一元运算符
    if (check(TokenType::DB_PLUS) || check(TokenType::DB_MINUS) || check(TokenType::DB_NOT)) {
        // 保存一元运算符 Token 的位置信息
        std::size_t line = current_token_.get_line();
        std::size_t column = current_token_.get_column();
        // 获取一元运算符 Token 类型
        TokenType operator_type = current_token_.get_type();

        // 消耗一元运算符
        advance();

        // 创建一元表达式节点
        auto unary_expression = std::make_unique<AstUnaryExpressionNode>(line, column);
        // 设置运算符类型为一元运算符
        if (operator_type == TokenType::DB_PLUS) {
            unary_expression->set_operator_type(AstUnaryOperatorType::AST_UNARY_OPERATOR_PLUS);
        } else if (operator_type == TokenType::DB_MINUS) {
            unary_expression->set_operator_type(AstUnaryOperatorType::AST_UNARY_OPERATOR_MINUS);
        } else if (operator_type == TokenType::DB_NOT) {
            unary_expression->set_operator_type(AstUnaryOperatorType::AST_UNARY_OPERATOR_NOT);
        } else {
            error("Unexpected operator in unary expression");
            return nullptr;
        }

        // 递归解析右侧的表达式
        auto right = parse_unary_expression();
        if (right == nullptr) {
            error("Expected expression after unary");
            return nullptr;
        }
        // 设置右操作数为右侧解析的结果
        unary_expression->set_operand(std::move(right));

        // 一元表达式解析完成
        return unary_expression;
    } else {
        // 不是一元运算符，继续解析主表达式
        return parse_primary_expression();
    }
}

std::unique_ptr<AstExpressionNode> Parser::parse_primary_expression()
{
    // 字面量解析
    if (check(TokenType::DB_INTEGER_LITERAL)) {
        // 获取整数字面量值
        std::string value_str = current_token_.get_value();
        
        // 消耗整数字面量
        advance();
        
        // 解析为整数
        try {
            std::int64_t int_value = std::stoll(value_str);
            return AstLiteralExpressionNode::create_integer(int_value);
        } catch (...) {
            error("Invalid integer literal");
            return nullptr;
        }
    }

    if (check(TokenType::DB_FLOAT_LITERAL)) {
        // 获取浮点数字面量值
        std::string value_str = current_token_.get_value();
        
        // 消耗浮点数字面量
        advance();
        
        // 解析为浮点数
        try {
            double float_value = std::stod(value_str);
            return AstLiteralExpressionNode::create_float(float_value);
        } catch (...) {
            error("Invalid float literal");
            return nullptr;
        }
    }

    if (check(TokenType::DB_STRING_LITERAL)) {
        // 获取字符串字面量位置信息
        std::string value = current_token_.get_value();
        
        // 消耗字符串字面量
        advance();
        
        return AstLiteralExpressionNode::create_string(value);
    }

    if (check(TokenType::DB_TRUE)) {
        // 消耗 TRUE 关键字
        advance();
        return AstLiteralExpressionNode::create_boolean(true);
    }

    if (check(TokenType::DB_FALSE)) {
        // 消耗 FALSE 关键字
        advance();
        return AstLiteralExpressionNode::create_boolean(false);
    }

    if (check(TokenType::DB_NULL)) {
        // 消耗 NULL 关键字
        advance();
        return AstLiteralExpressionNode::create_null();
    }

    // 标识符解析，包括列引用和函数调用
    if (check(TokenType::DB_IDENTIFIER)) {
        // 获取标识符位置信息
        std::size_t line = current_token_.get_line();
        std::size_t column = current_token_.get_column();

        // 检查下一个 Token 是否为 (
        // 注意 peek 不会移动 Token，检查完后当前 Token 仍然是标识符
        if (lexer_->peek_token().get_type() == TokenType::DB_LEFT_PAREN) {
            // 解析为函数调用
            // 创建函数调用表达式节点
            auto function_call_expression = std::make_unique<AstFunctionCallExpressionNode>(line, column);

            // 获取函数名
            std::string function_name = current_token_.get_value();
            // 消耗标识符
            advance();

            // 设置函数名
            function_call_expression->set_function_name(function_name);

            // 期望 (
            consume(TokenType::DB_LEFT_PAREN, "Expected '(' after function name");

            // 解析函数参数
            // 如果下一个 Token 是 )，则没有参数
            if (!check(TokenType::DB_RIGHT_PAREN)) {
                do {
                    auto argument = parse_expression();
                    if (argument == nullptr) {
                        error("Expected expression in function arguments");
                        return nullptr;
                    }
                    function_call_expression->add_argument(std::move(argument));
                } while (match(TokenType::DB_COMMA));
            }

            // 期望 )
            consume(TokenType::DB_RIGHT_PAREN, "Expected ')' after function arguments");
            
            // 函数调用表达式解析完成
            return function_call_expression;
        }

        // 不是函数调用，创建列引用表达式节点
        auto column_ref = std::make_unique<AstColumnReferenceExpressionNode>(line, column);

        // 解析可能的 database.collection.column 格式
        std::string first_part = current_token_.get_value();
        advance();

        if (match(TokenType::DB_DOT)) {
            // 有 . 符号，可能是 database.collection 或 collection.column
            if (!check(TokenType::DB_IDENTIFIER)) {
                error("Expected identifier after '.'");
                return nullptr;
            }
            std::string second_part = current_token_.get_value();
            advance();

            if (match(TokenType::DB_DOT)) {
                // 三个部分：database.collection.column
                if (!check(TokenType::DB_IDENTIFIER)) {
                    error("Expected identifier after second '.'");
                    return nullptr;
                }
                column_ref->set_database_name(first_part);
                column_ref->set_collection_name(second_part);
                column_ref->set_column_name(current_token_.get_value());
                advance();
            } else {
                // 两个部分：collection.column
                column_ref->set_collection_name(first_part);
                column_ref->set_column_name(second_part);
            }
        } else {
            // 只有一个部分：column
            column_ref->set_column_name(first_part);
        }

        return column_ref;
    }

    // 括号表达式解析
    if (match(TokenType::DB_LEFT_PAREN)) {
        // 解析括号中的表达式
        auto expression = parse_expression();
        if (expression == nullptr) {
            error("Expected expression after '('");
            return nullptr;
        }

        // 期望 )
        consume(TokenType::DB_RIGHT_PAREN, "Expected ')' after expression");
        return expression;
    }

    // 向量解析
    if (check(TokenType::DB_LEFT_BRACKET)) {
        // 获取向量表达式位置信息
        std::size_t line = current_token_.get_line();
        std::size_t column = current_token_.get_column();

        // 消耗 [
        advance();

        // 创建向量表达式节点
        auto vector_expression = std::make_unique<AstVectorExpressionNode>(line, column);

        // 如果下一个 Token 是 ]，则向量值为空
        if (match(TokenType::DB_RIGHT_BRACKET)) {
            return vector_expression;
        }

        // 解析向量元素
        do {
            auto element = parse_expression();
            if (element == nullptr) {
                error("Expected expression in vector");
                return nullptr;
            }
            vector_expression->add_element(std::move(element));
        } while (match(TokenType::DB_COMMA));

        // 期望 ]
        consume(TokenType::DB_RIGHT_BRACKET, "Expected ']' after vector elements");

        // 向量表达式解析完成
        return vector_expression;
    }

    // 如果都不匹配，返回错误
    error("Expected primary expression");
    return nullptr;
}

AstColumnDefinition Parser::parse_column_definition()
{
    AstColumnDefinition column_definition;

    // 解析列名
    if (!check(TokenType::DB_IDENTIFIER)) {
        error("Expected column_name after '('");
        return AstColumnDefinition();
    }
    std::string column_name = current_token_.get_value();
    // 消耗标识符
    advance();
    column_definition.set_name(column_name);

    // 解析类型
    if (!check(TokenType::DB_IDENTIFIER)) {
        error("Expected type_name after column_name");
        return AstColumnDefinition();
    }
    std::string type_name = current_token_.get_value();
    // 消耗标识符
    advance();
    // 设置类型名
    column_definition.set_type_name(type_name);

    // 查看是否有参数列表起始字符 (
    if (match(TokenType::DB_LEFT_PAREN)) {
        do {
            // 解析参数
            auto argument = parse_expression();
            // 添加参数
            column_definition.add_argument(std::move(argument));
        } while (match(TokenType::DB_COMMA));

        // 期望 )
        consume(TokenType::DB_RIGHT_PAREN, "Expected ')' after arguments");
    }

    // 解析列约束，列约束可以以任意顺序出现
    while (true) {
        if (match(TokenType::DB_NOT)) {
            // 解析 NOT NULL
            consume(TokenType::DB_NULL, "Expected NULL after NOT");
            // 添加 NOT NULL 修饰符
            column_definition.add_modifier(AstColumnModifier::AST_COLUMN_MODIFIER_NOT_NULL);
        } else if (match(TokenType::DB_PRIMARY)) {
            // 解析 PRIMARY KEY
            // 期望 KEY
            consume(TokenType::DB_KEY, "Expected KEY after PRIMARY");
            // 添加 PRIMARY KEY 修饰符
            column_definition.add_modifier(AstColumnModifier::AST_COLUMN_MODIFIER_PRIMARY_KEY);
        } else if (match(TokenType::DB_AUTO_INCREMENT)) {
            // 解析 AUTO_INCREMENT
            // 添加 AUTO_INCREMENT 修饰符
            column_definition.add_modifier(AstColumnModifier::AST_COLUMN_MODIFIER_AUTO_INCREMENT);
        } else if (match(TokenType::DB_UNIQUE)) {
            // 解析 UNIQUE
            // 添加 UNIQUE 修饰符
            column_definition.add_modifier(AstColumnModifier::AST_COLUMN_MODIFIER_UNIQUE);
        } else if (match(TokenType::DB_DEFAULT)) {
            // 解析默认值表达式
            auto default_value = parse_expression();
            // 添加 DEFAULT 修饰符
            column_definition.add_modifier(AstColumnModifier::AST_COLUMN_MODIFIER_DEFAULT);
            // 添加默认值表达式
            column_definition.add_default_value(std::move(default_value));
        } else {
            // 没有更多约束了
            break;
        }
    }

    // 检查是否存在 COMMENT
    if (match(TokenType::DB_COMMENT)) {
        // 解析 COMMENT
        if (!check(TokenType::DB_STRING_LITERAL)) {
            error("Expected string_literal after COMMENT");
            return AstColumnDefinition();
        }
        std::string comment = current_token_.get_value();
        // 消耗字符串
        advance();
        // 设置 COMMENT
        column_definition.set_comment(comment);
    }

    return column_definition;
}

std::unique_ptr<AstExpressionNode> Parser::parse_in_expression(std::unique_ptr<AstExpressionNode> left, bool is_not)
{
    // 获取 IN 关键字的位置
    std::size_t line = current_token_.get_line();
    std::size_t column = current_token_.get_column();

    // 消耗 IN 关键字
    advance();

    // 创建 IN 表达式节点
    auto in_expression = std::make_unique<AstInExpressionNode>(line, column);
    in_expression->set_left(std::move(left));
    in_expression->set_is_not(is_not);

    // 期望 (
    consume(TokenType::DB_LEFT_PAREN, "Expected '(' after IN");

    // 解析 IN 表达式值列表
    while (true) {
        // 解析 IN 表达式值
        auto value = parse_expression();
        if (value == nullptr) {
            error("Expected expression after IN");
            return nullptr;
        }
        // 添加 IN 表达式值
        in_expression->add_value(std::move(value));
        if (!match(TokenType::DB_COMMA)) {
            break;
        }
    }

    // 期望 )
    consume(TokenType::DB_RIGHT_PAREN, "Expected ')' after IN");

    // IN 表达式解析完成
    return in_expression;
}

std::unique_ptr<AstExpressionNode> Parser::parse_between_expression(std::unique_ptr<AstExpressionNode> left, bool is_not)
{
    // 获取 BETWEEN 关键字的位置
    std::size_t line = current_token_.get_line();
    std::size_t column = current_token_.get_column();

    // 消耗 BETWEEN 关键字
    advance();

    // 创建 BETWEEN 表达式节点
    auto between_expression = std::make_unique<AstBetweenExpressionNode>(line, column);
    between_expression->set_left(std::move(left));
    between_expression->set_is_not(is_not);

    // 解析 BETWEEN 表达式左值
    auto left_value = parse_expression();
    if (left_value == nullptr) {
        error("Expected expression after BETWEEN");
        return nullptr;
    }
    // 添加 BETWEEN 表达式左值
    between_expression->set_start(std::move(left_value));

    // 期望 AND
    consume(TokenType::DB_AND, "Expected AND after BETWEEN");

    // 解析 BETWEEN 表达式右值
    auto right_value = parse_expression();
    if (right_value == nullptr) {
        error("Expected expression after AND");
        return nullptr;
    }
    // 添加 BETWEEN 表达式右值
    between_expression->set_end(std::move(right_value));

    // BETWEEN 表达式解析完成
    return between_expression;
}

std::unique_ptr<AstExpressionNode> Parser::parse_like_expression(std::unique_ptr<AstExpressionNode> left, bool is_not)
{
    // 获取 LIKE 关键字的位置
    std::size_t line = current_token_.get_line();
    std::size_t column = current_token_.get_column();

    // 消耗 LIKE 关键字
    advance();

    // 创建 LIKE 表达式节点
    auto like_expression = std::make_unique<AstLikeExpressionNode>(line, column);
    like_expression->set_left(std::move(left));
    like_expression->set_is_not(is_not);

    // 解析 LIKE 表达式模式
    auto pattern = parse_expression();
    if (pattern == nullptr) {
        error("Expected expression after LIKE");
        return nullptr;
    }
    // 添加 LIKE 表达式模式
    like_expression->set_pattern(std::move(pattern));

    // LIKE 表达式解析完成
    return like_expression;
}

Token Parser::advance()
{
    current_token_ = lexer_->next_token();
    return current_token_;
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
        error(message);
        return;
    }
}

void Parser::skip_semicolon()
{
    match(TokenType::DB_SEMICOLON);
}

void Parser::error(const std::string & message)
{
    throw ParseException(message, current_token_.get_line(), current_token_.get_column());
}

} // namespace dreamdb
