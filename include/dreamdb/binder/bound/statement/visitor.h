#pragma once

namespace dreamdb::binder::bound
{

class BoundAlterStatement;
class BoundCreateStatement;
class BoundDeleteStatement;
class BoundDescribeStatement;
class BoundDropStatement;
class BoundInsertStatement;
class BoundSelectStatement;
class BoundShowStatement;
class BoundUpdateStatement;
class BoundUseStatement;

/**
 * @brief 绑定语句访问者
 */
class BoundStatementVisitor
{
public:
    virtual ~BoundStatementVisitor() noexcept = default;

public:
    virtual void visit(const BoundAlterStatement & alter_statement) = 0;

    virtual void visit(const BoundCreateStatement & create_statement) = 0;

    virtual void visit(const BoundDeleteStatement & delete_statement) = 0;

    virtual void visit(const BoundDescribeStatement & describe_statement) = 0;

    virtual void visit(const BoundDropStatement & drop_statement) = 0;

    virtual void visit(const BoundInsertStatement & insert_statement) = 0;

    virtual void visit(const BoundSelectStatement & select_statement) = 0;

    virtual void visit(const BoundShowStatement & show_statement) = 0;

    virtual void visit(const BoundUpdateStatement & update_statement) = 0;

    virtual void visit(const BoundUseStatement & use_statement) = 0;
};

} // namespace dreamdb::binder::bound
