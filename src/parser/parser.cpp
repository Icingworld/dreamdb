#include "dreamdb/parser/parser.h"

#include <vector>

#include "dreamdb/parser/lexer.h"
#include "dreamdb/parser/ast/statement/select.h"
#include "dreamdb/parser/ast/statement/insert.h"
#include "dreamdb/parser/ast/statement/update.h"
#include "dreamdb/parser/ast/statement/delete.h"
#include "dreamdb/parser/ast/statement/drop.h"
#include "dreamdb/parser/ast/statement/create.h"
#include "dreamdb/parser/ast/statement/alter.h"
#include "dreamdb/parser/ast/statement/use.h"
#include "dreamdb/parser/ast/statement/show.h"
#include "dreamdb/parser/ast/statement/describe.h"
#include "dreamdb/parser/ast/expression/binary.h"
#include "dreamdb/parser/ast/expression/unary.h"
#include "dreamdb/parser/ast/expression/in.h"
#include "dreamdb/parser/ast/expression/between.h"
#include "dreamdb/parser/ast/expression/like.h"
#include "dreamdb/parser/ast/expression/literal.h"
#include "dreamdb/parser/ast/expression/vector.h"
#include "dreamdb/parser/ast/expression/column_reference.h"
#include "dreamdb/parser/ast/expression/function_call.h"

namespace dreamdb::parser
{

ParseException::ParseException(const std::string & message, std::size_t line, std::size_t column)
    : std::runtime_error(message)
    , line_(line)
    , column_(column)
{
}

std::size_t ParseException::line() const noexcept
{
    return line_;
}

std::size_t ParseException::column() const noexcept
{
    return column_;
}

std::string ParseException::message() const noexcept
{
    return std::string(what()) + " at line " + std::to_string(line_) + ", column " + std::to_string(column_);
}

Parser::Parser(const std::string & input)
    : lexer_(std::make_unique<Lexer>(input))
    , current_token_(Token(TokenType::EoF, "", 0, 0))
{
}

Parser::Parser(std::unique_ptr<Lexer> lexer)
    : lexer_(std::move(lexer))
    , current_token_(Token(TokenType::EoF, "", 0, 0))
{
}

Parser::~Parser() noexcept = default;

std::unique_ptr<ast::AstStatement> Parser::parse()
{
    // 初始化：读取第一个 Token
    current_token_ = lexer_->next();

    // 检查是否为空输入
    if (check(TokenType::EoF)) {
        return nullptr;
    }

    // 解析语句
    auto statement = parse_statement();

    // 跳过可能存在的分号
    skip_semicolon();

    // 检查是否还有更多内容
    if (!check(TokenType::EoF)) {
        error("Unexpected token");
    }

    // 解析完毕
    return statement;
}

std::unique_ptr<ast::AstStatement> Parser::parse_statement()
{
    // 根据当前 Token 类型选择对应的语句解析函数
    switch (current_token_.type()) {
        case TokenType::Select:
            return parse_select_statement();
        case TokenType::Insert:
            return parse_insert_statement();
        case TokenType::Update:
            return parse_update_statement();
        case TokenType::Delete:
            return parse_delete_statement();
        case TokenType::Create:
            return parse_create_statement();
        case TokenType::Drop:
            return parse_drop_statement();
        case TokenType::Use:
            return parse_use_statement();
        case TokenType::Alter:
            return parse_alter_statement();
        case TokenType::Show:
            return parse_show_statement();
        case TokenType::Describe:
        case TokenType::Desc:
            return parse_describe_statement();
        default:
            error("Unexpected token");
            return nullptr;
    }
}

std::unique_ptr<ast::AstStatement> Parser::parse_select_statement()
{
    // 获取 SELECT 关键字的位置信息
    std::size_t line = current_token_.line();
    std::size_t column = current_token_.column();

    // 消耗 SELECT 关键字
    advance();

    // 解析选择项列表
    std::vector<ast::AstSelectItem> select_items;
    do {
        if (match(TokenType::Star)) {
            // 添加星号条目
            select_items.push_back(ast::AstSelectStarItem{});
        } else {
            // 解析表达式
            auto expr = parse_expression();

            // 解析可选的别名
            std::optional<std::string> alias = std::nullopt;
            if (match(TokenType::As)) {
                // 解析别名
                if (!check(TokenType::Identifier)) {
                    error("Expected alias after AS");
                }
                alias = current_token_.value();
                advance();
            }

            // 创建表达式条目
            ast::AstSelectExpressionItem expression_item(std::move(expr), std::move(alias));
            select_items.push_back(std::move(expression_item));
        }
    } while (match(TokenType::Comma));

    // 期望 FROM 关键字
    consume(TokenType::From, "Expected FROM after select items");

    // 解析集合名称
    if (!check(TokenType::Identifier)) {
        error("Expected collection_name after FROM");
    }
    std::string collection_name = current_token_.value();
    // 消耗标识符
    advance();

    // 解析可选的 WHERE 子句
    std::unique_ptr<ast::AstExpression> where = nullptr;
    if (match(TokenType::Where)) {
        where = parse_expression();
    }

    // 解析可选的 GROUP BY 子句
    std::vector<std::unique_ptr<ast::AstExpression>> group_by;
    if (match(TokenType::Group)) {
        // 期望 BY 关键字
        consume(TokenType::By, "Expected BY after GROUP");

        // 解析 GROUP BY 子句
        do {
            auto group_by_expression = parse_expression();
            group_by.push_back(std::move(group_by_expression));
        } while (match(TokenType::Comma));
    }

    // 解析可选的 HAVING 子句
    std::unique_ptr<ast::AstExpression> having = nullptr;
    if (match(TokenType::Having)) {
        having = parse_expression();
    }

    // 解析可选的 ORDER BY 子句
    std::vector<ast::AstOrderByItem> order_by;
    if (match(TokenType::Order)) {
        // 期望 BY 关键字
        consume(TokenType::By, "Expected BY after ORDER");

        // 解析 ORDER BY 子句
        do {
            auto order_by_clause = parse_expression();

            // 解析可选的方向
            dreamdb::Direction direction = dreamdb::Direction::ASC;
            if (match(TokenType::Desc)) {
                direction = dreamdb::Direction::DESC;
            } else if (match(TokenType::Asc)) {
                direction = dreamdb::Direction::ASC;
            }

            // 创建 ORDER BY 项
            ast::AstOrderByItem order_by_item(std::move(order_by_clause), direction);
            order_by.push_back(std::move(order_by_item));
        } while (match(TokenType::Comma));
    }

    // 解析可选的 LIMIT 子句
    std::optional<std::uint64_t> limit;
    if (match(TokenType::Limit)) {
        // 解析整数
        if (!check(TokenType::IntegerLiteral)) {
            error("Expected integer literal after LIMIT");
        }
        try {
            limit = std::stoull(current_token_.value());
        } catch (...) {
            error("Invalid integer literal for LIMIT");
        }
        advance();
    }

    // 解析可选的 OFFSET 子句
    std::optional<std::uint64_t> offset;
    if (match(TokenType::Offset)) {
        // 解析整数
        if (!check(TokenType::IntegerLiteral)) {
            error("Expected integer literal after OFFSET");
        }
        try {
            offset = std::stoull(current_token_.value());
        } catch (...) {
            error("Invalid integer literal for OFFSET");
        }
        advance();
    }

    // 创建 SELECT 语句
    return ast::AstSelectStatement::create(
        std::move(collection_name),
        std::move(select_items),
        std::move(where),
        std::move(group_by),
        std::move(having),
        std::move(order_by),
        limit,
        offset,
        line,
        column
    );
}

std::unique_ptr<ast::AstStatement> Parser::parse_insert_statement()
{
    // 获取 INSERT 关键字的位置信息
    std::size_t line = current_token_.line();
    std::size_t column = current_token_.column();

    // 消耗 INSERT 关键字
    advance();

    // 期望 INTO 关键字
    consume(TokenType::Into, "Expected INTO after INSERT");

    // 解析集合名称
    if (!check(TokenType::Identifier)) {
        error("Expected collection_name after INTO");
    }
    std::string collection_name = current_token_.value();
    // 消耗标识符
    advance();

    // 检查是否存在列名列表
    std::vector<std::string> column_names;
    if (match(TokenType::LeftParen)) {
        // 解析列名列表
        do {
            if (!check(TokenType::Identifier)) {
                error("Expected column_name after '('");
            }
            std::string column_name = current_token_.value();
            // 消耗标识符
            advance();
            // 添加列名
            column_names.push_back(std::move(column_name));
        } while (match(TokenType::Comma));

        // 期望 )
        consume(TokenType::RightParen, "Expected ')' after column_names");
    }

    // 期望 VALUES 关键字
    consume(TokenType::Values, "Expected VALUES after INTO");

    // 期望 (
    consume(TokenType::LeftParen, "Expected '(' after VALUES");

    // 解析值列表
    std::vector<std::unique_ptr<ast::AstExpression>> values;
    do {
        auto value = parse_expression();
        // 添加值
        values.push_back(std::move(value));
    } while (match(TokenType::Comma));

    // 期望 )
    consume(TokenType::RightParen, "Expected ')' after values");

    // 创建 INSERT 语句
    return ast::AstInsertStatement::create(
        std::move(collection_name),
        std::move(column_names),
        std::move(values),
        line,
        column
    );
}

std::unique_ptr<ast::AstStatement> Parser::parse_update_statement()
{
    // 获取 UPDATE 关键字的位置信息
    std::size_t line = current_token_.line();
    std::size_t column = current_token_.column();

    // 消耗 UPDATE 关键字
    advance();

    // 解析集合名称
    if (!check(TokenType::Identifier)) {
        error("Expected collection_name after UPDATE");
    }
    std::string collection_name = current_token_.value();
    // 消耗标识符
    advance();

    // 期望 SET 关键字
    consume(TokenType::Set, "Expected SET after UPDATE");

    // 解析赋值项列表，至少需要一项
    std::vector<ast::AstUpdateAssignment> assignments;
    do {
        if (!check(TokenType::Identifier)) {
            error("Expected column_name before =");
        }
        std::string column_name = current_token_.value();
        // 消耗标识符
        advance();

        // 期望 =
        consume(TokenType::Equal, "Expected = after column_name");

        // 解析值
        auto value = parse_expression();

        // 创建 AstUpdateAssignment 对象
        ast::AstUpdateAssignment assignment(column_name, std::move(value));
        // 添加 AstUpdateAssignment 对象
        assignments.push_back(std::move(assignment));
    } while (match(TokenType::Comma));

    // 解析可选的 WHERE 子句
    std::unique_ptr<ast::AstExpression> where = nullptr;
    if (match(TokenType::Where)) {
        where = parse_expression();
    }

    // 创建 UPDATE 语句
    return ast::AstUpdateStatement::create(
        std::move(collection_name),
        std::move(assignments),
        std::move(where),
        line,
        column
    );
}

std::unique_ptr<ast::AstStatement> Parser::parse_delete_statement()
{
    // 获取 DELETE 关键字的位置信息
    std::size_t line = current_token_.line();
    std::size_t column = current_token_.column();

    // 消耗 DELETE 关键字
    advance();

    // 期望 FROM 关键字
    consume(TokenType::From, "Expected FROM after DELETE");

    // 解析集合名称
    if (!check(TokenType::Identifier)) {
        error("Expected collection_name after FROM");
    }
    std::string collection_name = current_token_.value();
    // 消耗标识符
    advance();

    // 解析可选的 WHERE 子句
    std::unique_ptr<ast::AstExpression> where = nullptr;
    if (match(TokenType::Where)) {
        where = parse_expression();
    }

    // 创建 DELETE 语句
    return ast::AstDeleteStatement::create(
        std::move(collection_name), std::move(where), line, column
    );
}

std::unique_ptr<ast::AstStatement> Parser::parse_create_statement()
{
    // 获取 CREATE 关键字的位置信息
    std::size_t line = current_token_.line();
    std::size_t column = current_token_.column();

    // 消耗 CREATE 关键字
    advance();

    // 解析对象类型
    if (match(TokenType::Database)) {
        // 解析是否需要进行存在性检查
        bool if_not_exists = false;
        if (match(TokenType::If)) {
            consume(TokenType::Not, "Expected NOT after IF");
            consume(TokenType::Exists, "Expected EXISTS after NOT");
            if_not_exists = true;
        }

        // 解析数据库名称
        if (!check(TokenType::Identifier)) {
            error("Expected database_name after DATABASE");
        }
        std::string database_name = current_token_.value();
        // 消耗标识符
        advance();

        // 创建 AstCreateDatabase 操作
        ast::AstCreateDatabase create_database;
        create_database.database_name = std::move(database_name);

        // 创建 CREATE DATABASE 语句
        return ast::AstCreateStatement::create(
            std::move(create_database), if_not_exists, line, column
        );
    } else if (match(TokenType::Collection)) {
        // 解析是否需要进行存在性检查
        bool if_not_exists = false;
        if (match(TokenType::If)) {
            consume(TokenType::Not, "Expected NOT after IF");
            consume(TokenType::Exists, "Expected EXISTS after NOT");
            if_not_exists = true;
        }

        // 解析集合名称
        if (!check(TokenType::Identifier)) {
            error("Expected collection_name after COLLECTION");
        }
        std::string collection_name = current_token_.value();
        // 消耗标识符
        advance();

        // 期望 (
        consume(TokenType::LeftParen, "Expected '(' after collection_name");

        // 解析列定义列表
        std::vector<ast::AstColumnDefinition> column_definitions;
        do {
            auto column_definition = parse_column_definition();

            // 添加列定义
            column_definitions.push_back(std::move(column_definition));
        } while (match(TokenType::Comma));

        // 期望 )
        consume(TokenType::RightParen, "Expected ')' after column_definitions");

        // 创建 AstCreateCollection 操作
        ast::AstCreateCollection create_collection;
        create_collection.collection_name = std::move(collection_name);
        create_collection.column_definitions = std::move(column_definitions);

        // 创建 CREATE COLLECTION 语句
        return ast::AstCreateStatement::create(
            std::move(create_collection), if_not_exists, line, column
        );
    } else if (match(TokenType::Index)) {
        // 解析是否需要进行存在性检查
        bool if_not_exists = false;
        if (match(TokenType::If)) {
            consume(TokenType::Not, "Expected NOT after IF");
            consume(TokenType::Exists, "Expected EXISTS after NOT");
            if_not_exists = true;
        }

        // 解析索引名称
        if (!check(TokenType::Identifier)) {
            error("Expected index_name after INDEX");
        }
        std::string index_name = current_token_.value();
        // 消耗标识符
        advance();

        // 期望 ON 关键字
        consume(TokenType::On, "Expected ON after INDEX");

        // 解析集合名称
        if (!check(TokenType::Identifier)) {
            error("Expected collection_name after ON");
        }
        std::string collection_name = current_token_.value();
        // 消耗标识符
        advance();

        // 期望 (
        consume(TokenType::LeftParen, "Expected '(' after collection_name");

        // 解析列名列表
        std::vector<std::string> column_names;
        do {
            if (!check(TokenType::Identifier)) {
                error("Expected column_name after '('");
            }
            std::string column_name = current_token_.value();
            // 消耗标识符
            advance();

            // 添加列名
            column_names.push_back(std::move(column_name));
        } while (match(TokenType::Comma));

        // 期望 )
        consume(TokenType::RightParen, "Expected ')' after column_names");

        std::optional<std::string> index_type = std::nullopt;
        // 检查是否存在 USING
        if (match(TokenType::Using)) {
            // 解析索引类型
            if (!check(TokenType::Identifier)) {
                error("Expected index_type after USING");
            }
            index_type = std::make_optional(current_token_.value());
            // 消耗标识符
            advance();
        }

        // 创建 AstCreateIndex 操作
        ast::AstCreateIndex create_index;
        create_index.index_name = std::move(index_name);
        create_index.collection_name = std::move(collection_name);
        create_index.column_names = std::move(column_names);
        create_index.index_type = std::move(index_type);

        // 创建 CREATE INDEX 语句
        return ast::AstCreateStatement::create(
            std::move(create_index), if_not_exists, line, column
        );
    } else if (match(TokenType::VIndex)) {
        // 解析是否需要进行存在性检查
        bool if_not_exists = false;
        if (match(TokenType::If)) {
            consume(TokenType::Not, "Expected NOT after IF");
            consume(TokenType::Exists, "Expected EXISTS after NOT");
            if_not_exists = true;
        }

        // 解析向量索引名称
        if (!check(TokenType::Identifier)) {
            error("Expected vindex_name after VINDEX");
        }
        std::string vindex_name = current_token_.value();
        // 消耗标识符
        advance();

        // 期望 ON 关键字
        consume(TokenType::On, "Expected ON after VINDEX");

        // 解析集合名称
        if (!check(TokenType::Identifier)) {
            error("Expected collection_name after ON");
        }
        std::string collection_name = current_token_.value();
        // 消耗标识符
        advance();

        // 期望 (
        consume(TokenType::LeftParen, "Expected '(' after ON");

        // 解析列名
        if (!check(TokenType::Identifier)) {
            error("Expected column_name after '('");
        }
        std::string column_name = current_token_.value();
        // 消耗标识符
        advance();

        // 期望 )
        consume(TokenType::RightParen, "Expected ')' after column_name");

        // 查看是否存在 USING
        std::optional<std::string> vindex_type = std::nullopt;
        std::vector<ast::AstVIndexWithOption> with_clauses;
        if (match(TokenType::Using)) {
            // 解析向量索引类型
            if (!check(TokenType::Identifier)) {
                error("Expected vindex_type after USING");
            }
            vindex_type = current_token_.value();
            // 消耗标识符
            advance();

            // 查看是否存在 WITH
            if (match(TokenType::With)) {
                // 期望 (
                consume(TokenType::LeftParen, "Expected '(' after WITH");

                // 解析 WITH 子句
                do {
                    if (!check(TokenType::Identifier)) {
                        error("Expected key after WITH");
                    }
                    std::string key = current_token_.value();
                    // 消耗标识符
                    advance();

                    // 期望 =
                    consume(TokenType::Equal, "Expected = after key");

                    // 解析值
                    auto value = parse_expression();
                    // 添加 WITH 子句
                    with_clauses.push_back(ast::AstVIndexWithOption(key, std::move(value)));
                } while (match(TokenType::Comma));

                // 期望 )
                consume(TokenType::RightParen, "Expected ')' after WITH");
            }
        }

        // 创建 AstCreateVIndex 操作
        ast::AstCreateVIndex create_vindex;
        create_vindex.vindex_name = std::move(vindex_name);
        create_vindex.collection_name = std::move(collection_name);
        create_vindex.column_name = std::move(column_name);
        create_vindex.vindex_type = std::move(vindex_type);
        create_vindex.with_clauses = std::move(with_clauses);

        // 创建 CREATE VINDEX 语句
        return ast::AstCreateStatement::create(
            std::move(create_vindex), if_not_exists, line, column
        );
    } else {
        error("Expected DATABASE, COLLECTION, INDEX or VINDEX after CREATE");
    }
}

std::unique_ptr<ast::AstStatement> Parser::parse_drop_statement()
{
    // 获取 DROP 关键字的位置信息
    std::size_t line = current_token_.line();
    std::size_t column = current_token_.column();

    // 消耗 DROP 关键字
    advance();

    // 解析对象类型
    if (match(TokenType::Database)) {
        // 解析是否需要进行存在性检查
        bool if_exists = false;
        if (match(TokenType::If)) {
            consume(TokenType::Exists, "Expected EXISTS after IF");
            if_exists = true;
        }

        // 解析数据库名称
        if (!check(TokenType::Identifier)) {
            error("Expected database_name after DATABASE");
        }
        std::string database_name = current_token_.value();
        // 消耗标识符
        advance();

        // 创建 AstDropDatabase 操作
        ast::AstDropDatabase drop_database;
        drop_database.database_name = std::move(database_name);

        // 创建 DROP DATABASE 语句
        return ast::AstDropStatement::create(
            std::move(drop_database), if_exists, line, column
        );
    } else if (match(TokenType::Collection)) {
        // 解析是否需要进行存在性检查
        bool if_exists = false;
        if (match(TokenType::If)) {
            consume(TokenType::Exists, "Expected EXISTS after IF");
            if_exists = true;
        }

        // 解析集合名称
        if (!check(TokenType::Identifier)) {
            error("Expected collection_name after COLLECTION");
        }
        std::string collection_name = current_token_.value();
        // 消耗标识符
        advance();

        // 创建 AstDropCollection 操作
        ast::AstDropCollection drop_collection;
        drop_collection.collection_name = std::move(collection_name);

        // 创建 DROP COLLECTION 语句
        return ast::AstDropStatement::create(
            std::move(drop_collection), if_exists, line, column
        );
    } else if (match(TokenType::Index)) {
        // 解析是否需要进行存在性检查
        bool if_exists = false;
        if (match(TokenType::If)) {
            consume(TokenType::Exists, "Expected EXISTS after IF");
            if_exists = true;
        }

        // 解析索引名称
        if (!check(TokenType::Identifier)) {
            error("Expected index_name after INDEX");
        }
        std::string index_name = current_token_.value();
        // 消耗标识符
        advance();

        // 期望 ON 关键字
        consume(TokenType::On, "Expected ON after INDEX");

        // 解析集合名称
        if (!check(TokenType::Identifier)) {
            error("Expected collection_name after ON");
        }
        std::string collection_name = current_token_.value();
        // 消耗标识符
        advance();

        // 创建 AstDropIndex 操作
        ast::AstDropIndex drop_index;
        drop_index.index_name = std::move(index_name);
        drop_index.collection_name = std::move(collection_name);

        // 创建 DROP INDEX 语句
        return ast::AstDropStatement::create(
            std::move(drop_index), if_exists, line, column
        );
    } else if (match(TokenType::VIndex)) {
        // 解析是否需要进行存在性检查
        bool if_exists = false;
        if (match(TokenType::If)) {
            consume(TokenType::Exists, "Expected EXISTS after IF");
            if_exists = true;
        }

        // 解析向量索引名称
        if (!check(TokenType::Identifier)) {
            error("Expected vindex_name after VINDEX");
        }
        std::string vindex_name = current_token_.value();
        // 消耗标识符
        advance();

        // 期望 ON 关键字
        consume(TokenType::On, "Expected ON after VINDEX");

        // 解析集合名称
        if (!check(TokenType::Identifier)) {
            error("Expected collection_name after ON");
        }
        std::string collection_name = current_token_.value();
        // 消耗标识符
        advance();

        // 创建 AstDropVIndex 操作
        ast::AstDropVIndex drop_vindex;
        drop_vindex.vindex_name = std::move(vindex_name);
        drop_vindex.collection_name = std::move(collection_name);

        // 创建 DROP VINDEX 语句
        return ast::AstDropStatement::create(
            std::move(drop_vindex), if_exists, line, column
        );
    } else {
        error("Expected DATABASE, COLLECTION, INDEX or VINDEX after DROP");
    }
}

std::unique_ptr<ast::AstStatement> Parser::parse_use_statement()
{
    // 获取 USE 关键字的位置信息
    std::size_t line = current_token_.line();
    std::size_t column = current_token_.column();

    // 消耗 USE 关键字
    advance();

    // 解析数据库名称
    if (!check(TokenType::Identifier)) {
        error("Expected database_name after USE");
    }
    std::string database_name = current_token_.value();
    // 消耗标识符
    advance();

    // 创建 USE 语句
    return ast::AstUseStatement::create(
        std::move(database_name), line, column
    );
}

std::unique_ptr<ast::AstStatement> Parser::parse_alter_statement()
{
    // 获取 ALTER 关键字的位置信息
    std::size_t line = current_token_.line();
    std::size_t column = current_token_.column();

    // 消耗 ALTER 关键字
    advance();

    // 期望 COLLECTION 关键字
    consume(TokenType::Collection, "Expected COLLECTION after ALTER");

    // 解析集合名称
    if (!check(TokenType::Identifier)) {
        error("Expected collection_name after COLLECTION");
    }
    std::string collection_name = current_token_.value();
    // 消耗标识符
    advance();

    // 解析 ALTER 操作类型
    if (match(TokenType::Add)) {
        // 期望 COLUMN 关键字
        consume(TokenType::Column, "Expected COLUMN after ADD");

        // 解析列定义
        auto column_definition = parse_column_definition();

        // 创建 AstAlterAddColumn 操作
        ast::AstAlterAddColumn add_column{std::move(column_definition)};

        // 创建 ALTER ADD COLUMN 语句
        return ast::AstAlterStatement::create(
            std::move(collection_name), std::move(add_column), line, column
        );
    } else if (match(TokenType::Drop)) {
        // 期望 COLUMN 关键字
        consume(TokenType::Column, "Expected COLUMN after DROP");

        // 解析字段名称
        if (!check(TokenType::Identifier)) {
            error("Expected column_name after COLUMN");
        }
        std::string column_name = current_token_.value();
        // 消耗标识符
        advance();

        // 创建 AstAlterDropColumn 操作
        ast::AstAlterDropColumn drop_column;
        drop_column.column_name = std::move(column_name);

        // 创建 ALTER DROP COLUMN 语句
        return ast::AstAlterStatement::create(
            std::move(collection_name), std::move(drop_column), line, column
        );
    } else if (match(TokenType::Modify)) {
        // 期望 COLUMN 关键字
        consume(TokenType::Column, "Expected COLUMN after MODIFY");

        // 解析列定义
        auto new_definition = parse_column_definition();

        // 创建 AstAlterModifyColumn 操作
        ast::AstAlterModifyColumn modify_column{std::move(new_definition)};

        // 创建 ALTER MODIFY COLUMN 语句
        return ast::AstAlterStatement::create(
            std::move(collection_name), std::move(modify_column), line, column
        );
    } else if (match(TokenType::Rename)) {
        // 期望 COLUMN 关键字
        consume(TokenType::Column, "Expected COLUMN after RENAME");

        // 解析字段名称
        if (!check(TokenType::Identifier)) {
            error("Expected column_name after COLUMN");
        }
        std::string old_column_name = current_token_.value();
        // 消耗标识符
        advance();

        // 期望 TO 关键字
        consume(TokenType::To, "Expected TO after COLUMN");

        // 解析新字段名称
        if (!check(TokenType::Identifier)) {
            error("Expected new_column_name after TO");
        }
        std::string new_column_name = current_token_.value();
        // 消耗标识符
        advance();

        // 创建 AstAlterRenameColumn 操作
        ast::AstAlterRenameColumn rename_column;
        rename_column.old_name = std::move(old_column_name);
        rename_column.new_name = std::move(new_column_name);

        // 创建 ALTER RENAME COLUMN 语句
        return ast::AstAlterStatement::create(
            std::move(collection_name), std::move(rename_column), line, column
        );
    } else {
        error("Expected ADD, DROP, MODIFY or RENAME after collection_name");
    }
}

std::unique_ptr<ast::AstStatement> Parser::parse_show_statement()
{
    // 获取 SHOW 关键字的位置信息
    std::size_t line = current_token_.line();
    std::size_t column = current_token_.column();

    // 消耗 SHOW 关键字
    advance();

    // 解析对象类型
    if (match(TokenType::Databases)) {
        // 创建 AstShowDatabases 操作
        ast::AstShowDatabases show_databases;

        // 创建 SHOW DATABASES 语句
        return ast::AstShowStatement::create(
            show_databases, line, column
        );
    } else if (match(TokenType::Collections)) {
        // 创建 AstShowCollections 操作
        ast::AstShowCollections show_collections;

        // 判断是否指定数据库
        if (match(TokenType::From)) {
            // 解析数据库名称
            if (!check(TokenType::Identifier)) {
                error("Expected database_name after FROM");
            }
            std::string database_name = current_token_.value();
            // 消耗标识符
            advance();

            // 设置数据库名称
            show_collections.database_name = std::move(database_name);
        }

        // 创建 SHOW COLLECTIONS 语句
        return ast::AstShowStatement::create(
            std::move(show_collections), line, column
        );
    } else if (match(TokenType::Indexes)) {
        // 期望 FROM 关键字
        consume(TokenType::From, "Expected FROM after INDEXES");

        // 解析集合名称
        if (!check(TokenType::Identifier)) {
            error("Expected collection_name after FROM");
        }
        std::string collection_name = current_token_.value();
        // 消耗标识符
        advance();

        // 创建 AstShowIndexes 操作
        ast::AstShowIndexes show_indexes;
        show_indexes.collection_name = std::move(collection_name);

        // 判断是否指定数据库
        if (match(TokenType::From)) {
            // 解析数据库名称
            if (!check(TokenType::Identifier)) {
                error("Expected database_name after FROM");
            }
            std::string database_name = current_token_.value();
            // 消耗标识符
            advance();

            // 设置数据库名称
            show_indexes.database_name = std::move(database_name);
        }

        // 创建 SHOW INDEXES 语句
        return ast::AstShowStatement::create(
            std::move(show_indexes), line, column
        );
    } else if (match(TokenType::VIndexes)) {
        // 期望 FROM 关键字
        consume(TokenType::From, "Expected FROM after VINDEXES");

        // 解析集合名称
        if (!check(TokenType::Identifier)) {
            error("Expected collection_name after FROM");
        }
        std::string collection_name = current_token_.value();
        // 消耗标识符
        advance();

        // 创建 AstShowVIndexes 操作
        ast::AstShowVIndexes show_vindexes;
        show_vindexes.collection_name = std::move(collection_name);

        // 判断是否指定数据库
        if (match(TokenType::From)) {
            // 解析数据库名称
            if (!check(TokenType::Identifier)) {
                error("Expected database_name after FROM");
            }
            std::string database_name = current_token_.value();
            // 消耗标识符
            advance();

            // 设置数据库名称
            show_vindexes.database_name = std::move(database_name);
        }

        // 创建 SHOW VINDEXES 语句
        return ast::AstShowStatement::create(
            std::move(show_vindexes), line, column
        );
    } else {
        error("Expected DATABASES, COLLECTIONS, INDEXES or VINDEXES after SHOW");
    }
}

std::unique_ptr<ast::AstStatement> Parser::parse_describe_statement()
{
    // 获取 DESCRIBE 关键字的位置信息
    std::size_t line = current_token_.line();
    std::size_t column = current_token_.column();

    // 消耗 DESCRIBE 关键字
    advance();

    // 解析集合名称
    if (!check(TokenType::Identifier)) {
        error("Expected collection_name after DESCRIBE");
    }
    std::string collection_name = current_token_.value();
    // 消耗标识符
    advance();

    // 创建 DESCRIBE 语句
    return ast::AstDescribeStatement::create(
        std::move(collection_name), line, column
    );
}

std::unique_ptr<ast::AstExpression> Parser::parse_expression()
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

    // TODO: 评估是否下放入口判断到各个解析函数中
    // 考虑到可能存在某些 Token 在某些上下文中可以作为表达式起始，但在另外一些不行
    // 但目前这样设计足够使用

    // 能够进入递归下降解析表达式的 Token 类型
    switch (current_token_.type()) {
        // 字面量：数字、字符串、布尔值、NULL
        case TokenType::IntegerLiteral:
        case TokenType::FloatLiteral:
        case TokenType::StringLiteral:
        case TokenType::True:
        case TokenType::False:
        case TokenType::Null:
        // 标识符：字段名、变量等
        case TokenType::Identifier:
        // 一元运算符：+、-、NOT
        case TokenType::Plus:
        case TokenType::Minus:
        case TokenType::Not:
        // 括号表达式 '('
        case TokenType::LeftParen:
        // 向量字面量 '['
        case TokenType::LeftBracket:
            // 调用最低优先级解析函数
            return parse_or_expression();
        default:
            error("Expected expression");
    }
}

std::unique_ptr<ast::AstExpression> Parser::parse_or_expression()
{
    // 递归下降解析左侧的 AND 表达式
    auto left = parse_and_expression();

    // 左结合循环处理多个 OR 运算符
    // 例如: a OR b OR c 解析为 ((a OR b) OR c)
    while (check(TokenType::Or)) {
        // 保存 OR token 的位置信息
        std::size_t line = current_token_.line();
        std::size_t column = current_token_.column();

        // 消耗 OR 关键字
        advance();

        // 递归下降解析右侧的 AND 表达式
        auto right = parse_and_expression();

        // 创建二元表达式节点
        auto expr = ast::AstBinaryExpression::create(
            ast::AstBinaryOperatorType::Or,
            std::move(left),
            std::move(right),
            line,
            column
        );

        // 将当前表达式作为下一次的左操作数
        left = std::move(expr);
    }

    // OR 表达式解析完成
    return left;
}

std::unique_ptr<ast::AstExpression> Parser::parse_and_expression()
{
    // 递归下降解析左侧的比较表达式
    auto left = parse_comparison_expression();

    // 左结合循环处理多个 AND 运算符
    // 例如: a AND b AND c 解析为 ((a AND b) AND c)
    while (check(TokenType::And)) {
        // 保存 AND token 的位置信息
        std::size_t line = current_token_.line();
        std::size_t column = current_token_.column();

        // 消耗 AND 关键字
        advance();

        // 递归下降解析右侧的比较表达式
        auto right = parse_comparison_expression();

        // 创建二元表达式节点
        auto expr = ast::AstBinaryExpression::create(
            ast::AstBinaryOperatorType::And,
            std::move(left),
            std::move(right),
            line,
            column
        );

        // 将当前表达式作为下一次的左操作数
        left = std::move(expr);
    }

    // AND 表达式解析完成
    return left;
}

std::unique_ptr<ast::AstExpression> Parser::parse_comparison_expression()
{
    // 递归下降解析左侧的加法表达式
    auto left = parse_additive_expression();

    std::size_t line = current_token_.line();
    std::size_t column = current_token_.column();

    // 根据当前 Token 类型进行不同的比较表达式解析
    switch (current_token_.type()) {
        // 普通比较运算符：=、!=、<、>、<=、>=
        case TokenType::Equal:
        case TokenType::NotEqual:
        case TokenType::LessThan:
        case TokenType::GreaterThan:
        case TokenType::LessEqual:
        case TokenType::GreaterEqual: {
            // 确定比较运算符类型
            ast::AstBinaryOperatorType operator_type;
            switch (current_token_.type()) {
                case TokenType::Equal:
                    operator_type = ast::AstBinaryOperatorType::Equal;
                    break;
                case TokenType::NotEqual:
                    operator_type = ast::AstBinaryOperatorType::NotEqual;
                    break;
                case TokenType::LessThan:
                    operator_type = ast::AstBinaryOperatorType::LessThan;
                    break;
                case TokenType::GreaterThan:
                    operator_type = ast::AstBinaryOperatorType::GreaterThan;
                    break;
                case TokenType::LessEqual:
                    operator_type = ast::AstBinaryOperatorType::LessEqual;
                    break;
                case TokenType::GreaterEqual:
                    operator_type = ast::AstBinaryOperatorType::GreaterEqual;
                    break;
                default:
                    error("Unexpected comparison operator");
            }

            // 消耗比较运算符
            advance();

            // 递归解析右侧的加法表达式
            auto right = parse_additive_expression();

            // 创建二元表达式节点
            return ast::AstBinaryExpression::create(
                operator_type,
                std::move(left),
                std::move(right),
                line,
                column
            );
        }
        case TokenType::In:
            // 纯 IN 表达式 
            return parse_in_expression(std::move(left), false);
        case TokenType::Between:
            // 纯 BETWEEN 表达式
            return parse_between_expression(std::move(left), false);
        case TokenType::Like:
            // 纯 LIKE 表达式
            return parse_like_expression(std::move(left), false);
        case TokenType::Not:
            // NOT IN, NOT BETWEEN, NOT LIKE 表达式
            // 消耗 NOT 关键字
            advance();

            // 继续匹配 IN, BETWEEN, LIKE 表达式
            switch (current_token_.type()) {
                case TokenType::In:
                    // NOT IN 表达式
                    return parse_in_expression(std::move(left), true);
                case TokenType::Between:
                    // NOT BETWEEN 表达式
                    return parse_between_expression(std::move(left), true);
                case TokenType::Like:
                    // NOT LIKE 表达式
                    return parse_like_expression(std::move(left), true);
                default:
                    error("Unexpected IN, BETWEEN, LIKE after NOT");
            }
        default:
            return left;
    }
}

std::unique_ptr<ast::AstExpression> Parser::parse_additive_expression()
{
    // 递归下降解析左侧的乘法表达式
    auto left = parse_multiplicative_expression();

    // 左结合循环处理多个加法运算符
    // 例如: a + b + c 解析为 ((a + b) + c)
    while (check(TokenType::Plus) || check(TokenType::Minus)) {
        // 保存加号或减号 Token 的位置信息
        std::size_t line = current_token_.line();
        std::size_t column = current_token_.column();

        // 确定运算符类型
        ast::AstBinaryOperatorType operator_type;
        if (current_token_.type() == TokenType::Plus) {
            operator_type = ast::AstBinaryOperatorType::Plus;
        } else if (current_token_.type() == TokenType::Minus) {
            operator_type = ast::AstBinaryOperatorType::Minus;
        } else {
            error("Unexpected operator in additive expression");
        }

        // 消耗加号或减号关键字
        advance();

        // 递归解析右侧的乘法表达式
        auto right = parse_multiplicative_expression();

        // 创建二元表达式节点
        auto expr = ast::AstBinaryExpression::create(
            operator_type,
            std::move(left),
            std::move(right),
            line,
            column
        );

        // 将当前表达式作为下一次的左操作数
        left = std::move(expr);
    }

    // 加法表达式解析完成
    return left;
}

std::unique_ptr<ast::AstExpression> Parser::parse_multiplicative_expression()
{
    // 递归下降解析左侧的一元表达式
    auto left = parse_unary_expression();

    // 左结合循环处理多个乘法运算符
    // 例如: a * b * c 解析为 ((a * b) * c)
    // 注：Token 类型命名为 STAR 和 SLASH 是为了描述 * 和 / 的形态，而不是数学语义
    // 而 AstBinaryOperatorType 中使用 Multiply 和 Divide 是为了描述 * 和 / 的数学含义
    while (check(TokenType::Star) || check(TokenType::Slash) || check(TokenType::Modulo)) {
        // 保存乘号、除号或取模号 Token 的位置信息
        std::size_t line = current_token_.line();
        std::size_t column = current_token_.column();
        
        // 确定运算符类型
        ast::AstBinaryOperatorType operator_type;
        if (current_token_.type() == TokenType::Star) {
            operator_type = ast::AstBinaryOperatorType::Multiply;
        } else if (current_token_.type() == TokenType::Slash) {
            operator_type = ast::AstBinaryOperatorType::Divide;
        } else if (current_token_.type() == TokenType::Modulo) {
            operator_type = ast::AstBinaryOperatorType::Modulo;
        } else {
            error("Unexpected operator in multiplicative expression");
        }

        // 消耗乘号、除号或取模号关键字
        advance();

        // 递归解析右侧的一元表达式
        auto right = parse_unary_expression();

        // 创建二元表达式节点
        auto expr = ast::AstBinaryExpression::create(
            operator_type,
            std::move(left),
            std::move(right),
            line,
            column
        );

        // 将当前表达式作为下一次的左操作数
        left = std::move(expr);
    }

    // 乘法表达式解析完成
    return left;
}

std::unique_ptr<ast::AstExpression> Parser::parse_unary_expression()
{
    // 解析一元运算符
    if (check(TokenType::Plus) || check(TokenType::Minus) || check(TokenType::Not)) {
        // 保存一元运算符 Token 的位置信息
        std::size_t line = current_token_.line();
        std::size_t column = current_token_.column();

        // 确定运算符类型
        ast::AstUnaryOperatorType operator_type;
        if (current_token_.type() == TokenType::Plus) {
            operator_type = ast::AstUnaryOperatorType::Plus;
        } else if (current_token_.type() == TokenType::Minus) {
            operator_type = ast::AstUnaryOperatorType::Minus;
        } else if (current_token_.type() == TokenType::Not) {
            operator_type = ast::AstUnaryOperatorType::Not;
        } else {
            error("Unexpected operator in unary expression");
        }

        // 消耗一元运算符
        advance();

        // 递归解析操作数表达式（支持多个一元运算符，如 --x）
        auto operand = parse_unary_expression();

        // 创建一元表达式节点
        return ast::AstUnaryExpression::create(
            operator_type,
            std::move(operand),
            line,
            column
        );
    } else {
        // 不是一元运算符，继续解析主表达式
        return parse_primary_expression();
    }
}

std::unique_ptr<ast::AstExpression> Parser::parse_primary_expression()
{
    // 根据当前 Token 类型进行不同的解析
    switch (current_token_.type()) {
        // 字面量解析
        case TokenType::IntegerLiteral: {
            // 获取整数字面量值和位置信息
            std::string value_str = current_token_.value();
            std::size_t line = current_token_.line();
            std::size_t column = current_token_.column();

            // 消耗整数字面量
            advance();

            // 解析为整数
            try {
                std::int64_t int_value = std::stoll(value_str);
                return ast::AstLiteralExpression::create_integer(int_value, line, column);
            } catch (...) {
                error("Invalid integer literal");
            }
        }
        case TokenType::FloatLiteral: {
            // 获取浮点数字面量值和位置信息
            std::string value_str = current_token_.value();
            std::size_t line = current_token_.line();
            std::size_t column = current_token_.column();

            // 消耗浮点数字面量
            advance();

            // 解析为浮点数
            try {
                double float_value = std::stod(value_str);
                return ast::AstLiteralExpression::create_float(float_value, line, column);
            } catch (...) {
                error("Invalid float literal");
            }
        }
        case TokenType::StringLiteral: {
            // 获取字符串字面量值和位置信息
            std::string value = current_token_.value();
            std::size_t line = current_token_.line();
            std::size_t column = current_token_.column();

            // 消耗字符串字面量
            advance();

            return ast::AstLiteralExpression::create_string(value, line, column);
        }
        case TokenType::True: {
            // 获取位置信息
            std::size_t line = current_token_.line();
            std::size_t column = current_token_.column();

            // 消耗 TRUE 关键字
            advance();
            return ast::AstLiteralExpression::create_boolean(true, line, column);
        }
        case TokenType::False: {
            // 获取位置信息
            std::size_t line = current_token_.line();
            std::size_t column = current_token_.column();

            // 消耗 FALSE 关键字
            advance();
            return ast::AstLiteralExpression::create_boolean(false, line, column);
        }
        case TokenType::Null: {
            // 获取位置信息
            std::size_t line = current_token_.line();
            std::size_t column = current_token_.column();

            // 消耗 NULL 关键字
            advance();
            return ast::AstLiteralExpression::create_null(line, column);
        }
        // 标识符解析（列引用或函数调用）
        case TokenType::Identifier: {
            // 获取标识符位置信息
            std::size_t line = current_token_.line();
            std::size_t column = current_token_.column();
            std::string identifier = current_token_.value();

            // 消耗标识符
            advance();

            // 检查下一个 Token 是否为 (，判断是否为函数调用
            // 注意 peek() 不会移动 Token，检查完后当前 Token 仍然是标识符之后的下一个 Token
            if (check(TokenType::LeftParen)) {
                // 解析为函数调用
                return parse_function_call_expression(line, column, identifier);
            }

            // 不是函数调用，解析为列引用
            return parse_column_reference_expression(line, column, identifier);
        }
        // 括号表达式解析
        case TokenType::LeftParen: {
            // 消耗 (
            advance();

            // 解析括号中的表达式
            auto expression = parse_expression();

            // 期望 )
            consume(TokenType::RightParen, "Expected ')' after expression");
            return expression;
        }
        // 向量表达式解析
        case TokenType::LeftBracket: {
            // 获取向量表达式位置信息
            std::size_t line = current_token_.line();
            std::size_t column = current_token_.column();

            // 消耗 [
            advance();

            // 解析向量元素
            std::vector<std::unique_ptr<ast::AstExpression>> elements;

            // 如果下一个 Token 不是 ]，则解析元素
            if (!check(TokenType::RightBracket)) {
                do {
                    auto element = parse_expression();

                    elements.push_back(std::move(element));
                } while (match(TokenType::Comma));
            }

            // 期望 ]
            consume(TokenType::RightBracket, "Expected ']' after vector elements");

            // 创建向量表达式节点
            return ast::AstVectorExpression::create(std::move(elements), line, column);
        }
        default:
            // 如果都不匹配，返回错误
            error("Expected primary expression");
            return nullptr;
    }
}

std::unique_ptr<ast::AstExpression> Parser::parse_function_call_expression(
    std::size_t line,
    std::size_t column,
    const std::string & function_name
)
{
    // 期望 (
    consume(TokenType::LeftParen, "Expected '(' after function name");

    // 解析函数参数
    std::vector<std::unique_ptr<ast::AstExpression>> arguments;

    // 如果下一个 Token 不是 )，则解析参数
    if (!check(TokenType::RightParen)) {
        do {
            auto argument = parse_expression();

            arguments.push_back(std::move(argument));
        } while (match(TokenType::Comma));
    }

    // 期望 )
    consume(TokenType::RightParen, "Expected ')' after function arguments");

    // 创建函数调用表达式节点
    return ast::AstFunctionCallExpression::create(function_name, std::move(arguments), line, column);
}

// 解析列引用表达式的辅助函数
std::unique_ptr<ast::AstExpression> Parser::parse_column_reference_expression(
    std::size_t line,
    std::size_t column,
    const std::string & first_part
)
{
    // 检查是否有 . 符号，解析可能的 database.collection.column 格式
    if (match(TokenType::Dot)) {
        // 有 . 符号，可能是 database.collection 或 collection.column
        if (!check(TokenType::Identifier)) {
            error("Expected identifier after '.'");
        }
        std::string second_part = current_token_.value();
        advance();

        if (match(TokenType::Dot)) {
            // 三个部分：database.collection.column
            if (!check(TokenType::Identifier)) {
                error("Expected identifier after second '.'");
            }
            std::string third_part = current_token_.value();
            advance();
            return ast::AstColumnReferenceExpression::create(first_part, second_part, third_part, line, column);
        } else {
            // 两个部分：collection.column
            return ast::AstColumnReferenceExpression::create(first_part, second_part, line, column);
        }
    } else {
        // 只有一个部分：column
        return ast::AstColumnReferenceExpression::create(first_part, line, column);
    }
}

ast::AstColumnDefinition Parser::parse_column_definition()
{
    // 解析列名
    if (!check(TokenType::Identifier)) {
        error("Expected column_name");
    }
    std::string column_name = current_token_.value();
    advance();

    // 解析类型名
    if (!check(TokenType::Identifier)) {
        error("Expected type_name after column_name");
    }
    std::string type_name = current_token_.value();
    advance();

    // 解析类型参数列表（如果有）
    std::vector<std::unique_ptr<ast::AstExpression>> arguments;
    if (match(TokenType::LeftParen)) {
        // 如果下一个 Token 不是 )，则解析参数
        if (!check(TokenType::RightParen)) {
            do {
                auto argument = parse_expression();
                arguments.push_back(std::move(argument));
            } while (match(TokenType::Comma));
        }
        // 期望 )
        consume(TokenType::RightParen, "Expected ')' after type arguments");
    }

    // 解析列修饰符，列修饰符可以以任意顺序出现
    std::vector<ast::AstColumnModifier> modifiers;
    std::unique_ptr<ast::AstExpression> default_value = nullptr;

    while (true) {
        if (match(TokenType::Not)) {
            // 解析 NOT NULL
            consume(TokenType::Null, "Expected NULL after NOT");
            modifiers.push_back(ast::AstColumnModifier::NotNull);
        } else if (match(TokenType::Primary)) {
            // 解析 PRIMARY KEY
            consume(TokenType::Key, "Expected KEY after PRIMARY");
            modifiers.push_back(ast::AstColumnModifier::PrimaryKey);
        } else if (match(TokenType::AutoIncrement)) {
            // 解析 AUTO_INCREMENT
            modifiers.push_back(ast::AstColumnModifier::AutoIncrement);
        } else if (match(TokenType::Unique)) {
            // 解析 UNIQUE
            modifiers.push_back(ast::AstColumnModifier::Unique);
        } else if (match(TokenType::Default)) {
            // 解析 DEFAULT 默认值表达式
            default_value = parse_expression();
            modifiers.push_back(ast::AstColumnModifier::Default);
        } else {
            // 没有更多修饰符了
            break;
        }
    }

    // 解析可选的 COMMENT
    std::optional<std::string> comment = std::nullopt;
    if (match(TokenType::Comment)) {
        if (!check(TokenType::StringLiteral)) {
            error("Expected string_literal after COMMENT");
        }
        comment = current_token_.value();
        advance();
    }

    // 创建并返回列定义对象
    return ast::AstColumnDefinition(
        std::move(column_name),
        std::move(type_name),
        std::move(arguments),
        std::move(modifiers),
        std::move(default_value),
        comment
    );
}

std::unique_ptr<ast::AstExpression> Parser::parse_in_expression(std::unique_ptr<ast::AstExpression> left, bool is_not)
{
    // 获取 IN 关键字的位置
    std::size_t line = current_token_.line();
    std::size_t column = current_token_.column();

    // 消耗 IN 关键字
    advance();

    // 期望 (
    consume(TokenType::LeftParen, "Expected '(' after IN");

    // 解析 IN 表达式值列表
    std::vector<std::unique_ptr<ast::AstExpression>> values;

    // 如果下一个 Token 不是 )，则解析值
    if (!check(TokenType::RightParen)) {
        do {
            auto value = parse_expression();
            values.push_back(std::move(value));
        } while (match(TokenType::Comma));
    }

    // 期望 )
    consume(TokenType::RightParen, "Expected ')' after IN values");

    // 创建并返回 IN 表达式节点
    return ast::AstInExpression::create(
        std::move(left),
        std::move(values),
        is_not,
        line,
        column
    );
}

std::unique_ptr<ast::AstExpression> Parser::parse_between_expression(std::unique_ptr<ast::AstExpression> left, bool is_not)
{
    // 获取 BETWEEN 关键字的位置
    std::size_t line = current_token_.line();
    std::size_t column = current_token_.column();

    // 消耗 BETWEEN 关键字
    advance();

    // 解析 BETWEEN 表达式的起始值
    auto start = parse_expression();

    // 期望 AND
    consume(TokenType::And, "Expected AND after BETWEEN start value");

    // 解析 BETWEEN 表达式的结束值
    auto end = parse_expression();

    // 创建并返回 BETWEEN 表达式节点
    return ast::AstBetweenExpression::create(
        std::move(left),
        std::move(start),
        std::move(end),
        is_not,
        line,
        column
    );
}

std::unique_ptr<ast::AstExpression> Parser::parse_like_expression(std::unique_ptr<ast::AstExpression> left, bool is_not)
{
    // 获取 LIKE 关键字的位置
    std::size_t line = current_token_.line();
    std::size_t column = current_token_.column();

    // 消耗 LIKE 关键字
    advance();

    // 解析 LIKE 表达式的模式
    auto pattern = parse_expression();

    // 创建并返回 LIKE 表达式节点
    return ast::AstLikeExpression::create(
        std::move(left),
        std::move(pattern),
        is_not,
        line,
        column
    );
}

Token Parser::advance()
{
    current_token_ = lexer_->next();
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
    return current_token_.type() == type;
}

void Parser::consume(TokenType type, const std::string & message)
{
    if (check(type)) {
        advance();
    } else {
        error(message);
    }
}

void Parser::skip_semicolon()
{
    match(TokenType::Semicolon);
}

void Parser::error(const std::string & message)
{
    throw ParseException(message, current_token_.line(), current_token_.column());
}

} // namespace dreamdb::parser
