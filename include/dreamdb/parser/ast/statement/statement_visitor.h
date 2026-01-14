#pragma once

namespace dreamdb::parser::ast
{

class CreateStatement;
class DeleteStatement;
class InsertStatement;
class SelectStatement;
class UpdateStatement;
class UseStatement;
class ShowStatement;
class DescribeStatement;
class AlterStatement;

/**
 * @brief 语句访问者基类
 */
class AstStatementVisitor
{
public:
    virtual ~AstStatementVisitor() noexcept = default;

public:
    virtual void visit(const CreateStatement & statement) = 0;

    virtual void visit(const DeleteStatement & statement) = 0;

    virtual void visit(const InsertStatement & statement) = 0;

    virtual void visit(const SelectStatement & statement) = 0;

    virtual void visit(const UpdateStatement & statement) = 0;

    virtual void visit(const UseStatement & statement) = 0;

    virtual void visit(const ShowStatement & statement) = 0;

    virtual void visit(const DescribeStatement & statement) = 0;

    virtual void visit(const AlterStatement & statement) = 0;
};

} // namespace dreamdb::parser::ast
