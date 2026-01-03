#include "dreamdb/parser/ast/ast_insert_statement_node.h"

#include "dreamdb/parser/ast/ast_expression_node.h"

namespace dreamdb
{

AstInsertStatementNode::AstInsertStatementNode(std::size_t line, std::size_t column)
    : AstStatementNode(AstStatementNodeType::AST_STATEMENT_INSERT, line, column)
    , collection_name_(std::nullopt)
    , column_names_()
    , values_()
{
}

AstInsertStatementNode::~AstInsertStatementNode() noexcept = default;

void AstInsertStatementNode::set_collection_name(const std::string & collection_name)
{
    if (collection_name.empty()) {
        collection_name_ = std::nullopt;
    } else {
        collection_name_ = collection_name;
    }
}

void AstInsertStatementNode::add_column_name(const std::string & column)
{
    column_names_.push_back(column);
}

void AstInsertStatementNode::add_value(std::unique_ptr<AstExpressionNode> value)
{
    values_.emplace_back(std::move(value));
}

const std::string & AstInsertStatementNode::get_collection_name() const
{
    return collection_name_.value();
}

const std::vector<std::string> & AstInsertStatementNode::get_column_names() const noexcept
{
    return column_names_;
}

const std::vector<std::unique_ptr<AstExpressionNode>> & AstInsertStatementNode::get_values() const noexcept
{
    return values_;
}

bool AstInsertStatementNode::has_collection_name() const noexcept
{
    return collection_name_.has_value();
}

bool AstInsertStatementNode::has_column_names() const noexcept
{
    return !column_names_.empty();
}

bool AstInsertStatementNode::has_values() const noexcept
{
    return !values_.empty();
}

} // namespace dreamdb
