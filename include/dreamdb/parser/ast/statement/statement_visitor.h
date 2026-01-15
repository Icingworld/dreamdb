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
    virtual void visit(const AstAlterStatement & statement) = 0;

    virtual void visit(const AstCreateStatement & statement) = 0;

    virtual void visit(const AstDeleteStatement & statement) = 0;

    virtual void visit(const AstDescribeStatement & statement) = 0;

    virtual void visit(const AstDropStatement & statement) = 0;

    virtual void visit(const AstInsertStatement & statement) = 0;

    virtual void visit(const AstSelectStatement & statement) = 0;

    virtual void visit(const AstShowStatement & statement) = 0;

    virtual void visit(const AstUpdateStatement & statement) = 0;

    virtual void visit(const AstUseStatement & statement) = 0;
};

} // namespace dreamdb::parser::ast
