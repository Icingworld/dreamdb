#pragma once

namespace dreamdb::parser::ast
{

class AstCreateStatement;
class AstDeleteStatement;
class AstDropStatement;
class AstInsertStatement;
class AstSelectStatement;
class AstUpdateStatement;
class AstUseStatement;
class AstShowStatement;
class AstDescribeStatement;
class AstAlterStatement;

/**
 * @brief 语句访问者基类
 */
class AstStatementVisitor
{
public:
    virtual ~AstStatementVisitor() noexcept = default;

public:
    virtual void visit(const AstAlterStatement & alter_statement) = 0;

    virtual void visit(const AstCreateStatement & create_statement) = 0;

    virtual void visit(const AstDeleteStatement & delete_statement) = 0;

    virtual void visit(const AstDescribeStatement & describe_statement) = 0;

    virtual void visit(const AstDropStatement & drop_statement) = 0;

    virtual void visit(const AstInsertStatement & insert_statement) = 0;

    virtual void visit(const AstSelectStatement & select_statement) = 0;

    virtual void visit(const AstShowStatement & show_statement) = 0;

    virtual void visit(const AstUpdateStatement & update_statement) = 0;

    virtual void visit(const AstUseStatement & use_statement) = 0;
};

} // namespace dreamdb::parser::ast
