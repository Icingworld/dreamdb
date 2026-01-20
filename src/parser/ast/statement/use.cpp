#include "dreamdb/parser/ast/statement/use.h"

#include <cassert>

#include "dreamdb/parser/ast/statement/statement_visitor.h"

namespace dreamdb::parser::ast
{

AstUseStatement::AstUseStatement(std::string database_name, std::size_t line, std::size_t column)
    : AstStatement(AstStatementType::Use, line, column)
    , database_name_(database_name)
{
    // 数据库名称不能为空
    assert(!database_name.empty());
}

std::unique_ptr<AstUseStatement> AstUseStatement::create(
    std::string database_name,
    std::size_t line,
    std::size_t column
)
{
    return std::make_unique<AstUseStatement>(database_name, line, column);
}

const std::string & AstUseStatement::database_name() const noexcept
{
    return database_name_;
}

void AstUseStatement::accept(AstStatementVisitor & visitor) const
{
    visitor.visit(*this);
}

} // namespace dreamdb::parser::ast
