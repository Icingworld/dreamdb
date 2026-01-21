#include "dreamdb/binder/binder.h"

#include <stdexcept>

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

namespace dreamdb::binder
{

Binder::Binder(const dreamdb::catalog::Catalog & catalog)
    : catalog_(catalog)
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
    return nullptr;
}

std::unique_ptr<bound::BoundStatement> Binder::bind_create_statement(
    const dreamdb::parser::ast::AstCreateStatement & create_statement
)
{
    return nullptr;
}

std::unique_ptr<bound::BoundStatement> Binder::bind_delete_statement(
    const dreamdb::parser::ast::AstDeleteStatement & delete_statement
)
{
    return nullptr;
}

std::unique_ptr<bound::BoundStatement> Binder::bind_describe_statement(
    const dreamdb::parser::ast::AstDescribeStatement & describe_statement
)
{
    return nullptr;
}

std::unique_ptr<bound::BoundStatement> Binder::bind_drop_statement(
    const dreamdb::parser::ast::AstDropStatement & drop_statement
)
{
    return nullptr;
}

std::unique_ptr<bound::BoundStatement> Binder::bind_insert_statement(
    const dreamdb::parser::ast::AstInsertStatement & insert_statement
)
{
    return nullptr;
}

std::unique_ptr<bound::BoundStatement> Binder::bind_select_statement(
    const dreamdb::parser::ast::AstSelectStatement & select_statement
)
{
    return nullptr;
}

std::unique_ptr<bound::BoundStatement> Binder::bind_show_statement(
    const dreamdb::parser::ast::AstShowStatement & show_statement
)
{
    return nullptr;
}

std::unique_ptr<bound::BoundStatement> Binder::bind_update_statement(
    const dreamdb::parser::ast::AstUpdateStatement & update_statement
)
{
    return nullptr;
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

    // 创建 BoundUse 语句
    return std::make_unique<bound::BoundUseStatement>(database_id.value());
}

} // dreamdb::binder
