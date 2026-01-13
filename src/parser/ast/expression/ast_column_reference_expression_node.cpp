#include "dreamdb/parser/ast/ast_column_reference_expression_node.h"

namespace dreamdb
{

AstColumnReferenceExpressionNode::AstColumnReferenceExpressionNode(std::size_t line, std::size_t column)
    : AstExpressionNode(AstExpressionNodeType::AST_EXPRESSION_COLUMN_REFERENCE, line, column)
    , database_name_(std::nullopt)
    , collection_name_(std::nullopt)
    , column_name_(std::nullopt)
{
}

void AstColumnReferenceExpressionNode::set_database_name(const std::string & database_name)
{
    database_name_ = database_name;
}

void AstColumnReferenceExpressionNode::set_collection_name(const std::string & collection_name)
{
    collection_name_ = collection_name;
}

void AstColumnReferenceExpressionNode::set_column_name(const std::string & column_name)
{
    column_name_ = column_name;
}

const std::string & AstColumnReferenceExpressionNode::get_database_name() const
{
    return database_name_.value();
}

const std::string & AstColumnReferenceExpressionNode::get_collection_name() const
{
    return collection_name_.value();
}

const std::string & AstColumnReferenceExpressionNode::get_column_name() const
{
    return column_name_.value();
}

bool AstColumnReferenceExpressionNode::has_database_name() const noexcept
{
    return database_name_.has_value();
}

bool AstColumnReferenceExpressionNode::has_collection_name() const noexcept
{
    return collection_name_.has_value();
}

bool AstColumnReferenceExpressionNode::has_column_name() const noexcept
{
    return column_name_.has_value();
}

} // namespace dreamdb
