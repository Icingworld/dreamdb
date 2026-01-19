#pragma once

#include <string>
#include <sstream>

#include "dreamdb/parser/ast/expression/expression.h"
#include "dreamdb/parser/ast/statement/statement_visitor.h"
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
    void visit(const AstAlterStatement & statement) override;

    void visit(const AstCreateStatement & statement) override;

    void visit(const AstDeleteStatement & statement) override;

    void visit(const AstDescribeStatement & statement) override;

    void visit(const AstDropStatement & statement) override;

    void visit(const AstInsertStatement & statement) override;

    void visit(const AstSelectStatement & statement) override;

    void visit(const AstShowStatement & statement) override;

    void visit(const AstUpdateStatement & statement) override;

    void visit(const AstUseStatement & statement) override;

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
