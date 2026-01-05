#include "dreamdb/parser/parser.h"

#include "dreamdb/parser/ast/ast_drop_statement_node.h"
#include "dreamdb/parser/ast/ast_create_statement_node.h"
#include "dreamdb/parser/ast/ast_use_statement_node.h"
#include "dreamdb/parser/ast/ast_show_statement_node.h"
#include "dreamdb/parser/ast/ast_describe_statement_node.h"

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
            AstColumnDefinition column_definition;

            // 解析列名
            if (!check(TokenType::DB_IDENTIFIER)) {
                error("Expected column_name after '('");
                return nullptr;
            }
            std::string column_name = current_token_.get_value();
            // 消耗标识符
            advance();
            column_definition.set_name(column_name);

            // 解析类型
            if (!check(TokenType::DB_IDENTIFIER)) {
                error("Expected type_name after column_name");
                return nullptr;
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
                    return nullptr;
                }
                std::string comment = current_token_.get_value();
                // 消耗字符串
                advance();
                // 设置 COMMENT
                column_definition.set_comment(comment);
            }

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

} // namespace dreamdb
