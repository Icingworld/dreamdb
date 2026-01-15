#include "dreamdb/parser/ast/statement/delete_statement.h"

#include <cassert>

#include "dreamdb/parser/ast/expression/expression.h"
#include "dreamdb/parser/ast/statement/statement_visitor.h"

namespace dreamdb::parser::ast
{

AstDeleteStatement::AstDeleteStatement(
    std::string collection_name,
    std::unique_ptr<AstExpression> where,
    std::size_t line,
    std::size_t column
)
    : AstStatement(line, column)
    , collection_name_(std::move(collection_name))
    , where_(std::move(where))
{
    // 集合名称不能为空
    assert(!collection_name_.empty());
}

std::unique_ptr<AstDeleteStatement> AstDeleteStatement::create(
    std::string collection_name,
    std::unique_ptr<AstExpression> where,
    std::size_t line,
    std::size_t column
)
{
    return std::make_unique<AstDeleteStatement>(
        std::move(collection_name),
        std::move(where),
        line,
        column
    );
}

const std::string & AstDeleteStatement::collection_name() const noexcept
{
    return collection_name_;
}

bool AstDeleteStatement::has_where() const noexcept
{
    return where_ != nullptr;
}

const AstExpression * AstDeleteStatement::where() const noexcept
{
    return where_.get();
}

const AstExpression & AstDeleteStatement::where_ref() const noexcept
{
    // WHERE 子句必须存在
    assert(where_ != nullptr);

    return *where_;
}

void AstDeleteStatement::accept(AstStatementVisitor & visitor) const
{
    visitor.visit(*this);
}

} // namespace dreamdb::parser::ast

