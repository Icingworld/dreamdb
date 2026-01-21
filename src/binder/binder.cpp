#include "dreamdb/binder/binder.h"

#include <stdexcept>
#include <unordered_set>

#include "dreamdb/catalog/catalog.h"
#include "dreamdb/parser/ast/statement/statement.h"
#include "dreamdb/parser/ast/statement/alter.h"
#include "dreamdb/parser/ast/statement/create.h"
#include "dreamdb/parser/ast/statement/delete.h"
#include "dreamdb/parser/ast/statement/describe.h"
#include "dreamdb/parser/ast/statement/drop.h"
#include "dreamdb/parser/ast/statement/insert.h"
#include "dreamdb/parser/ast/statement/select.h"
#include "dreamdb/parser/ast/statement/show.h"
#include "dreamdb/parser/ast/statement/update.h"
#include "dreamdb/parser/ast/statement/use.h"
#include "dreamdb/binder/bound/statement/statement.h"
#include "dreamdb/binder/bound/statement/alter.h"
#include "dreamdb/binder/bound/statement/create.h"
#include "dreamdb/binder/bound/statement/delete.h"
#include "dreamdb/binder/bound/statement/describe.h"
#include "dreamdb/binder/bound/statement/drop.h"
#include "dreamdb/binder/bound/statement/insert.h"
#include "dreamdb/binder/bound/statement/select.h"
#include "dreamdb/binder/bound/statement/show.h"
#include "dreamdb/binder/bound/statement/update.h"
#include "dreamdb/binder/bound/statement/use.h"
#include "dreamdb/binder/bound/expression/expression.h"
#include "dreamdb/binder/bound/expression/column_reference.h"

namespace dreamdb::binder
{

Binder::Binder(const dreamdb::catalog::Catalog & catalog)
    : catalog_(catalog)
    , context_({std::nullopt, std::nullopt})
{
}

std::unique_ptr<bound::BoundStatement> Binder::bind(const dreamdb::parser::ast::AstStatement & statement)
{
    switch (statement.statement_type()) {
        case dreamdb::parser::ast::AstStatementType::Alter:
            return bind_alter_statement(
                static_cast<const dreamdb::parser::ast::AstAlterStatement &>(statement)
            );
        case dreamdb::parser::ast::AstStatementType::Create:
            return bind_create_statement(
                static_cast<const dreamdb::parser::ast::AstCreateStatement &>(statement)
            );
        case dreamdb::parser::ast::AstStatementType::Delete:
            return bind_delete_statement(
                static_cast<const dreamdb::parser::ast::AstDeleteStatement &>(statement)
            );
        case dreamdb::parser::ast::AstStatementType::Describe:
            return bind_describe_statement(
                static_cast<const dreamdb::parser::ast::AstDescribeStatement &>(statement)
            );
        case dreamdb::parser::ast::AstStatementType::Drop:
            return bind_drop_statement(
                static_cast<const dreamdb::parser::ast::AstDropStatement &>(statement)
            );
        case dreamdb::parser::ast::AstStatementType::Insert:
            return bind_insert_statement(
                static_cast<const dreamdb::parser::ast::AstInsertStatement &>(statement)
            );
        case dreamdb::parser::ast::AstStatementType::Select:
            return bind_select_statement(
                static_cast<const dreamdb::parser::ast::AstSelectStatement &>(statement)
            );
        case dreamdb::parser::ast::AstStatementType::Show:
            return bind_show_statement(
                static_cast<const dreamdb::parser::ast::AstShowStatement &>(statement)
            );
        case dreamdb::parser::ast::AstStatementType::Update:
            return bind_update_statement(
                static_cast<const dreamdb::parser::ast::AstUpdateStatement &>(statement)
            );
        case dreamdb::parser::ast::AstStatementType::Use:
            return bind_use_statement(
                static_cast<const dreamdb::parser::ast::AstUseStatement &>(statement)
            );
        default:
            throw std::runtime_error("Unknown statement type");
    }
}

std::unique_ptr<bound::BoundStatement> Binder::bind_alter_statement(
    const dreamdb::parser::ast::AstAlterStatement & alter_statement
)
{
    (void)alter_statement;
    return nullptr;
}

std::unique_ptr<bound::BoundStatement> Binder::bind_create_statement(
    const dreamdb::parser::ast::AstCreateStatement & create_statement
)
{
    (void)create_statement;
    return nullptr;
}

std::unique_ptr<bound::BoundStatement> Binder::bind_delete_statement(
    const dreamdb::parser::ast::AstDeleteStatement & delete_statement
)
{
    // 获取集合名称
    const std::string & collection_name = delete_statement.collection_name();

    // 检查当前数据库是否存在
    if (!context_.current_database_id.has_value()) {
        throw std::runtime_error("Current database not set");
    }

    // 检查集合是否存在
    auto collection_id = catalog_.resolve_collection(
        context_.current_database_id.value(), collection_name
    );
    if (!collection_id.has_value()) {
        throw std::runtime_error("Collection " + collection_name + " not found");
    }

    // 获取 WHERE 子句
    if (!delete_statement.has_where()) {
        // 不包含 WHERE 子句，删除所有数据
        return std::make_unique<bound::BoundDeleteStatement>(
            collection_id.value(), nullptr
        );
    }

    // 绑定 WHERE 子句
    auto bound_where = bind_expression(delete_statement.where_ref());

    // 创建 BoundDeleteStatement 语句
    return std::make_unique<bound::BoundDeleteStatement>(collection_id.value(), std::move(bound_where));
}

std::unique_ptr<bound::BoundStatement> Binder::bind_describe_statement(
    const dreamdb::parser::ast::AstDescribeStatement & describe_statement
)
{
    // 获取集合名称
    const std::string & collection_name = describe_statement.collection_name();

    // 检查当前数据库是否存在
    // TODO: 删除上下文测试代码，当前数据库将在 CommandExecutor 中设置
    if (!context_.current_database_id.has_value()) {
        throw std::runtime_error("Current database not set");
    }

    // 检查集合是否存在
    auto collection_id = catalog_.resolve_collection(
        context_.current_database_id.value(), collection_name
    );
    if (!collection_id.has_value()) {
        throw std::runtime_error("Collection " + collection_name + " not found");
    }

    // 创建 BoundDescribe 语句
    return std::make_unique<bound::BoundDescribeStatement>(collection_id.value());
}

std::unique_ptr<bound::BoundStatement> Binder::bind_drop_statement(
    const dreamdb::parser::ast::AstDropStatement & drop_statement
)
{
    // 获取操作
    const auto & ast_operation = drop_statement.operation();
    return std::visit([this, &drop_statement](const auto & op) -> std::unique_ptr<bound::BoundStatement> {
        using T = std::decay_t<decltype(op)>;

        if constexpr (std::is_same_v<T, dreamdb::parser::ast::AstDropDatabase>) {
            // 获取数据库名称
            const std::string & database_name = op.database_name;
            // 获取是否有存在性检查
            bool if_exists = drop_statement.if_exists();

            // 创建 BoundDropDatabase 操作
            bound::BoundDropDatabase bound_operation;

            // 检查数据库是否存在
            auto database_id = catalog_.resolve_database(database_name);
            if (!database_id.has_value()) {
                if (if_exists) {
                    // 不存在但是有存在性检查
                    bound_operation.database_id = std::nullopt;
                } else {
                    // 不存在且没有存在性检查
                    throw std::runtime_error("Database " + database_name + " not found");
                }
            } else {
                // 存在
                bound_operation.database_id = database_id.value();
            }

            // 创建 BoundDropStatement 语句
            return std::make_unique<bound::BoundDropStatement>(std::move(bound_operation));
        } else if constexpr (std::is_same_v<T, dreamdb::parser::ast::AstDropCollection>) {
            // 获取集合名称
            const std::string & collection_name = op.collection_name;

            // 获取是否有存在性检查
            bool if_exists = drop_statement.if_exists();

            // 创建 BoundDropCollection 操作
            bound::BoundDropCollection bound_operation;

            // 检查当前数据库是否存在
            if (!context_.current_database_id.has_value()) {
                throw std::runtime_error("Current database not set");
            }

            // 检查集合是否存在
            auto collection_id = catalog_.resolve_collection(
                context_.current_database_id.value(), collection_name
            );
            if (!collection_id.has_value()) {
                if (if_exists) {
                    // 不存在但是有存在性检查
                    bound_operation.collection_id = std::nullopt;
                } else {
                    // 不存在且没有存在性检查
                    throw std::runtime_error("Collection " + collection_name + " not found");
                }
            } else {
                // 存在
                bound_operation.collection_id = collection_id.value();
            }

            // 创建 BoundDropStatement 语句
            return std::make_unique<bound::BoundDropStatement>(std::move(bound_operation));
        } else if constexpr (std::is_same_v<T, dreamdb::parser::ast::AstDropIndex>) {
            // 获取索引名称
            const std::string & index_name = op.index_name;
            // 获取集合名称
            const std::string & collection_name = op.collection_name;

            // 获取是否有存在性检查
            bool if_exists = drop_statement.if_exists();

            // 创建 BoundDropIndex 操作
            bound::BoundDropIndex bound_operation;

            // 检查当前数据库是否存在
            if (!context_.current_database_id.has_value()) {
                throw std::runtime_error("Current database not set");
            }

            // 解析集合
            auto collection_id = catalog_.resolve_collection(
                context_.current_database_id.value(), collection_name
            );
            if (!collection_id.has_value()) {
                if (if_exists) {
                    bound_operation.index_id = std::nullopt;
                    return std::make_unique<bound::BoundDropStatement>(std::move(bound_operation));
                }
                throw std::runtime_error("Collection " + collection_name + " not found");
            }

            // 解析索引
            auto index_id = catalog_.resolve_index(collection_id.value(), index_name);
            if (!index_id.has_value()) {
                if (if_exists) {
                    bound_operation.index_id = std::nullopt;
                } else {
                    throw std::runtime_error("Index " + index_name + " not found");
                }
            } else {
                bound_operation.index_id = index_id.value();
            }

            // 创建 BoundDropStatement 语句
            return std::make_unique<bound::BoundDropStatement>(std::move(bound_operation));
        } else if constexpr (std::is_same_v<T, dreamdb::parser::ast::AstDropVIndex>) {
            // 获取向量索引名称
            const std::string & vindex_name = op.vindex_name;
            // 获取集合名称
            const std::string & collection_name = op.collection_name;

            // 获取是否有存在性检查
            bool if_exists = drop_statement.if_exists();

            // 创建 BoundDropVIndex 操作
            bound::BoundDropVIndex bound_operation;

            // 检查当前数据库是否存在
            if (!context_.current_database_id.has_value()) {
                throw std::runtime_error("Current database not set");
            }

            // 解析集合
            auto collection_id = catalog_.resolve_collection(
                context_.current_database_id.value(), collection_name
            );
            if (!collection_id.has_value()) {
                if (if_exists) {
                    bound_operation.vindex_id = std::nullopt;
                    return std::make_unique<bound::BoundDropStatement>(std::move(bound_operation));
                }
                throw std::runtime_error("Collection " + collection_name + " not found");
            }

            // 解析向量索引
            auto vindex_id = catalog_.resolve_vindex(collection_id.value(), vindex_name);
            if (!vindex_id.has_value()) {
                if (if_exists) {
                    bound_operation.vindex_id = std::nullopt;
                } else {
                    throw std::runtime_error("VIndex " + vindex_name + " not found");
                }
            } else {
                bound_operation.vindex_id = vindex_id.value();
            }

            // 创建 BoundDropStatement 语句
            return std::make_unique<bound::BoundDropStatement>(std::move(bound_operation));
        }
    }, ast_operation);
}

std::unique_ptr<bound::BoundStatement> Binder::bind_insert_statement(
    const dreamdb::parser::ast::AstInsertStatement & insert_statement
)
{
    (void)insert_statement;
    return nullptr;
}

std::unique_ptr<bound::BoundStatement> Binder::bind_select_statement(
    const dreamdb::parser::ast::AstSelectStatement & select_statement
)
{
    (void)select_statement;
    return nullptr;
}

std::unique_ptr<bound::BoundStatement> Binder::bind_show_statement(
    const dreamdb::parser::ast::AstShowStatement & show_statement
)
{
    // 获取操作
    const auto & ast_operation = show_statement.operation();
    return std::visit([this](const auto & op) -> std::unique_ptr<bound::BoundStatement> {
        using T = std::decay_t<decltype(op)>;

        if constexpr (std::is_same_v<T, dreamdb::parser::ast::AstShowDatabases>) {
            // 创建 BoundShowDatabases 操作
            bound::BoundShowDatabases bound_operation;

            // 创建 BoundShowStatement 语句
            return std::make_unique<bound::BoundShowStatement>(std::move(bound_operation));
        }
        else if constexpr (std::is_same_v<T, dreamdb::parser::ast::AstShowCollections>) {
            // 创建 BoundShowCollections 操作
            bound::BoundShowCollections bound_operation;

            // 如果指定了数据库名称，解析数据库 ID
            if (op.database_name.has_value()) {
                auto database_id = catalog_.resolve_database(op.database_name.value());
                if (!database_id.has_value()) {
                    throw std::runtime_error("Database " + op.database_name.value() + " not found");
                }
                bound_operation.database_id = database_id.value();
            } else {
                // 如果没有指定数据库名称，设置为 nullopt，让 Executor 使用当前数据库
                bound_operation.database_id = std::nullopt;
            }

            // 创建 BoundShowStatement 语句
            return std::make_unique<bound::BoundShowStatement>(std::move(bound_operation));
        }
        else if constexpr (std::is_same_v<T, dreamdb::parser::ast::AstShowIndexes>) {
            // 创建 BoundShowIndexes 操作
            bound::BoundShowIndexes bound_operation;

            // 确定数据库 ID（用于解析集合名称）
            dreamdb::common::database_id_t database_id;
            if (op.database_name.has_value()) {
                // 如果指定了数据库名称，解析数据库 ID
                auto resolved_db_id = catalog_.resolve_database(op.database_name.value());
                if (!resolved_db_id.has_value()) {
                    throw std::runtime_error("Database " + op.database_name.value() + " not found");
                }
                database_id = resolved_db_id.value();
            } else {
                // 如果没有指定数据库名称，使用当前数据库
                if (!context_.current_database_id.has_value()) {
                    throw std::runtime_error("Current database not set");
                }
                database_id = context_.current_database_id.value();
            }

            // 解析集合 ID（全局唯一）
            auto collection_id = catalog_.resolve_collection(database_id, op.collection_name);
            if (!collection_id.has_value()) {
                throw std::runtime_error("Collection " + op.collection_name + " not found");
            }

            bound_operation.collection_id = collection_id.value();

            // 创建 BoundShowStatement 语句
            return std::make_unique<bound::BoundShowStatement>(std::move(bound_operation));
        }
        else if constexpr (std::is_same_v<T, dreamdb::parser::ast::AstShowVIndexes>) {
            // 创建 BoundShowVIndexes 操作
            bound::BoundShowVIndexes bound_operation;

            // 确定数据库 ID（用于解析集合名称）
            dreamdb::common::database_id_t database_id;
            if (op.database_name.has_value()) {
                // 如果指定了数据库名称，解析数据库 ID
                auto resolved_db_id = catalog_.resolve_database(op.database_name.value());
                if (!resolved_db_id.has_value()) {
                    throw std::runtime_error("Database " + op.database_name.value() + " not found");
                }
                database_id = resolved_db_id.value();
            } else {
                // 如果没有指定数据库名称，使用当前数据库
                if (!context_.current_database_id.has_value()) {
                    throw std::runtime_error("Current database not set");
                }
                database_id = context_.current_database_id.value();
            }

            // 解析集合 ID（全局唯一）
            auto collection_id = catalog_.resolve_collection(database_id, op.collection_name);
            if (!collection_id.has_value()) {
                throw std::runtime_error("Collection " + op.collection_name + " not found");
            }

            bound_operation.collection_id = collection_id.value();

            // 创建 BoundShowStatement 语句
            return std::make_unique<bound::BoundShowStatement>(std::move(bound_operation));
        }
    }, ast_operation);
}

std::unique_ptr<bound::BoundStatement> Binder::bind_update_statement(
    const dreamdb::parser::ast::AstUpdateStatement & update_statement
)
{
    // 获取集合名称
    const std::string & collection_name = update_statement.collection_name();

    // 检查当前数据库是否存在
    if (!context_.current_database_id.has_value()) {
        throw std::runtime_error("Current database not set");
    }

    // 检查集合是否存在
    auto collection_id = catalog_.resolve_collection(
        context_.current_database_id.value(), collection_name
    );
    if (!collection_id.has_value()) {
        throw std::runtime_error("Collection " + collection_name + " not found");
    }

    // 获取 UPDATE 子句
    std::unordered_set<dreamdb::common::column_id_t> update_column_ids;
    std::vector<bound::BoundUpdateItem> update_items;
    for (std::size_t i = 0; i < update_statement.assignment_count(); ++i) {
        const auto & assignment = update_statement.assignment_at(i);

        // 获取列名称
        const std::string & column_name = assignment.column_name;

        // 获取列 ID
        auto column_info = catalog_.resolve_column(
            collection_id.value(), column_name
        );
        if (!column_info.has_value()) {
            throw std::runtime_error("Column " + column_name + " not found");
        }
        auto column_id = column_info->id;

        // 判断列是否重复
        if (update_column_ids.contains(column_id)) {
            throw std::runtime_error("Column " + column_name + " is duplicated");
        }

        // 绑定值表达式
        auto bound_value = bind_expression(*assignment.value);
        
        // 添加到列 ID 集合
        update_column_ids.insert(column_id);

        // 添加到更新项列表
        update_items.push_back(bound::BoundUpdateItem{
            std::make_unique<bound::BoundColumnReferenceExpression>(
                column_id, column_info->logical_type
            ),
            std::move(bound_value)
        });
    }

    // 获取 WHERE 子句
    if (!update_statement.has_where()) {
        // 不包含 WHERE 子句，更新所有数据
        return std::make_unique<bound::BoundUpdateStatement>(
            collection_id.value(), std::move(update_items), nullptr
        );
    }

    // 绑定 WHERE 子句
    auto bound_where = bind_expression(update_statement.where_ref());

    // 创建 BoundUpdateStatement 语句
    return std::make_unique<bound::BoundUpdateStatement>(
        collection_id.value(), std::move(update_items), std::move(bound_where)
    );
}

std::unique_ptr<bound::BoundStatement> Binder::bind_use_statement(
    const dreamdb::parser::ast::AstUseStatement & use_statement
)
{
    // 获取数据库名称
    const std::string & database_name = use_statement.database_name();

    // 检查数据库是否存在
    auto database_id = catalog_.resolve_database(database_name);
    if (!database_id.has_value()) {
        throw std::runtime_error("Database " + database_name + " not found");
    }

    // 更新上下文
    // TODO: 删除上下文测试代码，更新上下文将在 CommandExecutor 中进行
    context_.current_database_name = database_name;
    context_.current_database_id = database_id.value();

    // 创建 BoundUse 语句
    return std::make_unique<bound::BoundUseStatement>(database_id.value());
}

std::unique_ptr<bound::BoundExpression> Binder::bind_expression(
    const dreamdb::parser::ast::AstExpression & expression
)
{
    (void)expression;
    return nullptr;
}

} // dreamdb::binder
