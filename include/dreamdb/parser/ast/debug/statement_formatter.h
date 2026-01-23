#pragma once

#include <string>
#include <sstream>

#include "dreamdb/parser/ast/expression/expression.h"
#include "dreamdb/parser/ast/statement/visitor.h"
#include "dreamdb/parser/ast/statement/column_definition.h"
#include "dreamdb/parser/ast/debug/expression_formatter.h"

namespace dreamdb::parser::ast
{

class AstStatement;

/**
 * @brief 语句格式化器
 */
class AstStatementFormatter final : public AstStatementVisitor
{
public:
    AstStatementFormatter() noexcept;

    ~AstStatementFormatter() noexcept override = default;

public:
    /**
     * @brief 格式化语句
     * @param statement 语句
     * @return 格式化后的语句
     */
    std::string format(const AstStatement & statement);

private:
    void visit(const AstAlterStatement & alter_statement) override;

    void visit(const AstCreateStatement & create_statement) override;

    void visit(const AstDeleteStatement & delete_statement) override;

    void visit(const AstDescribeStatement & describe_statement) override;

    void visit(const AstDropStatement & drop_statement) override;

    void visit(const AstInsertStatement & insert_statement) override;

    void visit(const AstSelectStatement & select_statement) override;

    void visit(const AstShowStatement & show_statement) override;

    void visit(const AstUpdateStatement & update_statement) override;

    void visit(const AstUseStatement & use_statement) override;

    /**
     * @brief 格式化列定义
     * @param column_def 列定义
     */
    void format_column_definition(const AstColumnDefinition & column_def);

private:
    AstExpressionFormatter expression_formatter_;    // 表达式格式化器
    std::ostringstream oss_;                         // 格式化后的语句字符串流
};

} // namespace dreamdb::parser::ast
