#include "dreamdb/parser/ast/expression/column_reference_expression.h"

#include <cassert>

#include "dreamdb/parser/ast/expression/expression_visitor.h"

namespace dreamdb::parser::ast
{

AstColumnReferenceExpression::AstColumnReferenceExpression(
    std::optional<std::string> database_name,
    std::optional<std::string> collection_name,
    std::string column_name,
    std::size_t line,
    std::size_t column
)
    : AstExpression(line, column)
    , database_name_(std::move(database_name))
    , collection_name_(std::move(collection_name))
    , column_name_(std::move(column_name))
{
    // 列名不能为空
    assert(!column_name_.empty());
}

std::unique_ptr<AstColumnReferenceExpression> AstColumnReferenceExpression::create(
    std::string database_name,
    std::string collection_name,
    std::string column_name,
    std::size_t line,
    std::size_t column
)
{
    // 数据库名不能为空
    assert(!database_name.empty());

    // 集合名不能为空
    assert(!collection_name.empty());

    return std::unique_ptr<AstColumnReferenceExpression>(
        new AstColumnReferenceExpression(
            std::move(database_name),
            std::move(collection_name),
            std::move(column_name),
            line,
            column
        )
    );
}

std::unique_ptr<AstColumnReferenceExpression> AstColumnReferenceExpression::create(
    std::string collection_name,
    std::string column_name,
    std::size_t line,
    std::size_t column
)
{
    // 集合名不能为空
    assert(!collection_name.empty());

    return std::unique_ptr<AstColumnReferenceExpression>(
        new AstColumnReferenceExpression(
            std::nullopt,
            std::move(collection_name),
            std::move(column_name),
            line,
            column
        )
    );
}

std::unique_ptr<AstColumnReferenceExpression> AstColumnReferenceExpression::create(
    std::string column_name,
    std::size_t line,
    std::size_t column
)
{
    return std::unique_ptr<AstColumnReferenceExpression>(
        new AstColumnReferenceExpression(
            std::nullopt,
            std::nullopt,
            std::move(column_name),
            line,
            column
        )
    );
}

const std::optional<std::string> & AstColumnReferenceExpression::database_name() const noexcept
{
    return database_name_;
}

const std::optional<std::string> & AstColumnReferenceExpression::collection_name() const noexcept
{
    return collection_name_;
}

const std::string & AstColumnReferenceExpression::column_name() const noexcept
{
    return column_name_;
}

void AstColumnReferenceExpression::accept(AstExpressionVisitor & visitor) const
{
    visitor.visit(*this);
}

} // namespace dreamdb::parser::ast
