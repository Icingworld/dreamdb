#include "dreamdb/parser/ast/ast_show_statement_node.h"

namespace dreamdb
{

AstShowCollections::AstShowCollections()
    : database_name_(std::nullopt)
{
}

void AstShowCollections::set_database_name(const std::string & database_name)
{
    database_name_ = database_name;
}

const std::string & AstShowCollections::get_database_name() const
{
    return database_name_.value();
}

bool AstShowCollections::has_database_name() const noexcept
{
    return database_name_.has_value();
}

AstShowIndexes::AstShowIndexes(const std::string & collection_name)
    : collection_name_(collection_name)
    , database_name_(std::nullopt)
{
}

void AstShowIndexes::set_database_name(const std::string & database_name)
{
    database_name_ = database_name;
}

const std::string & AstShowIndexes::get_collection_name() const noexcept
{
    return collection_name_;
}

const std::string & AstShowIndexes::get_database_name() const
{
    return database_name_.value();
}

bool AstShowIndexes::has_database_name() const noexcept
{
    return database_name_.has_value();
}

AstShowVIndexes::AstShowVIndexes(const std::string & collection_name)
    : collection_name_(collection_name)
    , database_name_(std::nullopt)
{
}

void AstShowVIndexes::set_database_name(const std::string & database_name)
{
    database_name_ = database_name;
}

const std::string & AstShowVIndexes::get_collection_name() const noexcept
{
    return collection_name_;
}

const std::string & AstShowVIndexes::get_database_name() const
{
    return database_name_.value();
}

bool AstShowVIndexes::has_database_name() const noexcept
{
    return database_name_.has_value();
}

AstShowStatementNode::AstShowStatementNode(std::size_t line, std::size_t column)
    : AstStatementNode(AstStatementNodeType::AST_STATEMENT_SHOW, line, column)
    , show_operation_(std::monostate())
{
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
