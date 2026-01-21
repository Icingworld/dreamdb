#include "dreamdb/binder/bound/debug/statement_formatter.h"

#include "dreamdb/binder/bound/statement/statement.h"
#include "dreamdb/binder/bound/statement/describe.h"
#include "dreamdb/binder/bound/statement/use.h"

namespace dreamdb::binder::bound
{

BoundStatementFormatter::BoundStatementFormatter() noexcept
    : expression_formatter_()
    , oss_()
{
}

std::string BoundStatementFormatter::format(const BoundStatement & statement)
{
    oss_.str("");
    oss_.clear();
    statement.accept(*this);
    return oss_.str();
}

void BoundStatementFormatter::visit(const BoundAlterStatement & alter_statement)
{
    // TODO: 实现 BoundAlterStatement 格式化
    (void)alter_statement;
}

void BoundStatementFormatter::visit(const BoundCreateStatement & create_statement)
{
    // TODO: 实现 BoundCreateStatement 格式化
    (void)create_statement;
}

void BoundStatementFormatter::visit(const BoundDeleteStatement & delete_statement)
{
    // TODO: 实现 BoundDeleteStatement 格式化
    (void)delete_statement;
}

void BoundStatementFormatter::visit(const BoundDescribeStatement & describe_statement)
{
    oss_ << "DESCRIBE collection_id:" << describe_statement.collection_id();
}

void BoundStatementFormatter::visit(const BoundDropStatement & drop_statement)
{
    // TODO: 实现 BoundDropStatement 格式化
    (void)drop_statement;
}

void BoundStatementFormatter::visit(const BoundInsertStatement & insert_statement)
{
    // TODO: 实现 BoundInsertStatement 格式化
    (void)insert_statement;
}

void BoundStatementFormatter::visit(const BoundSelectStatement & select_statement)
{
    // TODO: 实现 BoundSelectStatement 格式化
    (void)select_statement;
}

void BoundStatementFormatter::visit(const BoundShowStatement & show_statement)
{
    // TODO: 实现 BoundShowStatement 格式化
    (void)show_statement;
}

void BoundStatementFormatter::visit(const BoundUpdateStatement & update_statement)
{
    // TODO: 实现 BoundUpdateStatement 格式化
    (void)update_statement;
}

void BoundStatementFormatter::visit(const BoundUseStatement & use_statement)
{
    oss_ << "USE database_id:" << use_statement.database_id();
}

} // namespace dreamdb::binder::bound
