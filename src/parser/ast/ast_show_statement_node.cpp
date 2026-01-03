#include "dreamdb/parser/ast/ast_show_statement_node.h"

namespace dreamdb
{

AstShowIndexes::AstShowIndexes(const std::string & collection_name)
    : collection_name_(collection_name)
{
}

const std::string & AstShowIndexes::get_collection_name() const noexcept
{
    return collection_name_;
}

AstShowVIndexes::AstShowVIndexes(const std::string & collection_name)
    : collection_name_(collection_name)
{
}

const std::string & AstShowVIndexes::get_collection_name() const noexcept
{
    return collection_name_;
}

AstShowStatementNode::AstShowStatementNode(std::size_t line, std::size_t column)
    : AstStatementNode(AstStatementNodeType::AST_STATEMENT_SHOW, line, column)
    , show_type_(AstShowType::AST_SHOW_UNKNOWN)
    , show_operation_(std::monostate())
{
}

void AstShowStatementNode::set_show_type(AstShowType show_type) noexcept
{
    show_type_ = show_type;
}

void AstShowStatementNode::set_show_databases(AstShowDatabases && op)
{
    show_operation_ = std::move(op);
}

void AstShowStatementNode::set_show_collections(AstShowCollections && op)
{
    show_operation_ = std::move(op);
}

void AstShowStatementNode::set_show_indexes(AstShowIndexes && op)
{
    show_operation_ = std::move(op);
}

void AstShowStatementNode::set_show_vindexes(AstShowVIndexes && op)
{
    show_operation_ = std::move(op);
}

AstShowType AstShowStatementNode::get_show_type() const noexcept
{
    return show_type_;
}

const AstShowDatabases & AstShowStatementNode::get_show_databases() const
{
    return std::get<AstShowDatabases>(show_operation_);
}

const AstShowCollections & AstShowStatementNode::get_show_collections() const
{
    return std::get<AstShowCollections>(show_operation_);
}

const AstShowIndexes & AstShowStatementNode::get_show_indexes() const
{
    return std::get<AstShowIndexes>(show_operation_);
}

const AstShowVIndexes & AstShowStatementNode::get_show_vindexes() const
{
    return std::get<AstShowVIndexes>(show_operation_);
}

bool AstShowStatementNode::has_show_operation() const noexcept
{
    return !std::holds_alternative<std::monostate>(show_operation_);
}

bool AstShowStatementNode::has_show_databases() const noexcept
{
    return std::holds_alternative<AstShowDatabases>(show_operation_);
}

bool AstShowStatementNode::has_show_collections() const noexcept
{
    return std::holds_alternative<AstShowCollections>(show_operation_);
}

bool AstShowStatementNode::has_show_indexes() const noexcept
{
    return std::holds_alternative<AstShowIndexes>(show_operation_);
}

bool AstShowStatementNode::has_show_vindexes() const noexcept
{
    return std::holds_alternative<AstShowVIndexes>(show_operation_);
}

} // namespace dreamdb
