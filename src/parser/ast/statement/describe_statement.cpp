#include "dreamdb/parser/ast/statement/describe_statement.h"

#include <cassert>

#include "dreamdb/parser/ast/statement/statement_visitor.h"

namespace dreamdb::parser::ast
{

AstDescribeStatement::AstDescribeStatement(
    std::string collection_name,
    std::size_t line,
    std::size_t column
)
    : AstStatement(line, column)
    , collection_name_(collection_name)
{
    // 集合名称不能为空
    assert(!collection_name.empty());
}

std::unique_ptr<AstDescribeStatement> AstDescribeStatement::create(
    std::string collection_name,
    std::size_t line,
    std::size_t column
)
{
    return std::make_unique<AstDescribeStatement>(collection_name, line, column);
}

const std::string & AstDescribeStatement::collection_name() const noexcept
{
    return collection_name_;
}

void AstDescribeStatement::accept(AstStatementVisitor & visitor) const
{
    visitor.visit(*this);
}

} // namespace dreamdb::parser::ast
