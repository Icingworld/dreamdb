#include "dreamdb/parser/ast/statement/insert_statement.h"

#include <cassert>

#include "dreamdb/parser/ast/expression/expression.h"
#include "dreamdb/parser/ast/statement/statement_visitor.h"

namespace dreamdb::parser::ast
{

AstInsertStatement::AstInsertStatement(
    std::string collection_name,
    std::vector<std::string> column_names,
    std::vector<std::unique_ptr<AstExpression>> values,
    std::size_t line,
    std::size_t column
)
    : AstStatement(AstStatementNodeType::Insert, line, column)
    , collection_name_(std::move(collection_name))
    , column_names_(std::move(column_names))
    , values_(std::move(values))
{
    // 集合名称不能为空
    assert(!collection_name_.empty());

    // 值列表不能为空
    assert(!values_.empty());

    // 每个值表达式都不能为空
    for (const auto & value : values_) {
        assert(value != nullptr);
    }
}

std::unique_ptr<AstInsertStatement> AstInsertStatement::create(
    std::string collection_name,
    std::vector<std::string> column_names,
    std::vector<std::unique_ptr<AstExpression>> values,
    std::size_t line,
    std::size_t column
)
{
    return std::make_unique<AstInsertStatement>(
        std::move(collection_name),
        std::move(column_names),
        std::move(values),
        line,
        column
    );
}

const std::string & AstInsertStatement::collection_name() const noexcept
{
    return collection_name_;
}

bool AstInsertStatement::has_column_names() const noexcept
{
    return !column_names_.empty();
}

const std::string & AstInsertStatement::column_name_at(std::size_t index) const noexcept
{
    // 索引不能超出范围
    assert(index < column_names_.size());

    return column_names_[index];
}

std::size_t AstInsertStatement::column_name_count() const noexcept
{
    return column_names_.size();
}

const AstExpression & AstInsertStatement::value_at(std::size_t index) const noexcept
{
    // 索引不能超出范围
    assert(index < values_.size());

    return *values_[index];
}

std::size_t AstInsertStatement::value_count() const noexcept
{
    return values_.size();
}

void AstInsertStatement::accept(AstStatementVisitor & visitor) const
{
    visitor.visit(*this);
}

} // namespace dreamdb::parser::ast
