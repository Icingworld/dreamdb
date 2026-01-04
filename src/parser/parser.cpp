#include "dreamdb/parser/parser.h"

#include "dreamdb/parser/ast/ast_drop_statement_node.h"
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
