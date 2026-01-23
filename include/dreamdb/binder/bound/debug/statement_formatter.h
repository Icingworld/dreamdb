#pragma once

#include <string>
#include <sstream>

#include "dreamdb/binder/bound/expression/expression.h"
#include "dreamdb/binder/bound/statement/visitor.h"
#include "dreamdb/binder/bound/debug/expression_formatter.h"

namespace dreamdb::binder::bound
{

class BoundStatement;

/**
 * @brief 绑定语句格式化器
 */
class BoundStatementFormatter final : public BoundStatementVisitor
{
public:
    BoundStatementFormatter() noexcept;

    ~BoundStatementFormatter() noexcept override = default;

public:
    /**
     * @brief 格式化语句
     * @param statement 语句
     * @return 格式化后的语句
     */
    std::string format(const BoundStatement & statement);

private:
    void visit(const BoundAlterStatement & alter_statement) override;

    void visit(const BoundCreateStatement & create_statement) override;

    void visit(const BoundDeleteStatement & delete_statement) override;

    void visit(const BoundDescribeStatement & describe_statement) override;

    void visit(const BoundDropStatement & drop_statement) override;

    void visit(const BoundInsertStatement & insert_statement) override;

    void visit(const BoundSelectStatement & select_statement) override;

    void visit(const BoundShowStatement & show_statement) override;

    void visit(const BoundUpdateStatement & update_statement) override;

    void visit(const BoundUseStatement & use_statement) override;

private:
    BoundExpressionFormatter expression_formatter_;    // 表达式格式化器
    std::ostringstream oss_;                           // 格式化后的语句字符串流
};

} // namespace dreamdb::binder::bound
