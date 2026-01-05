#include "dreamdb/parser/ast/ast_drop_statement_node.h"

namespace dreamdb
{

AstDropDatabase::AstDropDatabase(const std::string & database_name)
    : database_name_(database_name)
{
}

const std::string & AstDropDatabase::get_database_name() const noexcept
{
    return database_name_;
}

AstDropCollection::AstDropCollection(const std::string & collection_name)
    : collection_name_(collection_name)
{
}

const std::string & AstDropCollection::get_collection_name() const noexcept
{
    return collection_name_;
}

AstDropIndex::AstDropIndex(const std::string & index_name, const std::string & collection_name)
    : index_name_(index_name)
    , collection_name_(collection_name)
{
}

const std::string & AstDropIndex::get_index_name() const noexcept
{
    return index_name_;
}

const std::string & AstDropIndex::get_collection_name() const noexcept
{
    return collection_name_;
}

AstDropVIndex::AstDropVIndex(const std::string & vindex_name, const std::string & collection_name)
    : vindex_name_(vindex_name)
    , collection_name_(collection_name)
{
}

const std::string & AstDropVIndex::get_vindex_name() const noexcept
{
    return vindex_name_;
}

const std::string & AstDropVIndex::get_collection_name() const noexcept
{
    return collection_name_;
}

AstDropStatementNode::AstDropStatementNode(std::size_t line, std::size_t column)
    : AstStatementNode(AstStatementNodeType::AST_STATEMENT_DROP, line, column)
    , if_exists_(false)
    , drop_operation_(std::monostate())
{
}

void AstDropStatementNode::set_if_exists(bool if_exists) noexcept
{
    if_exists_ = if_exists;
}

void AstDropStatementNode::set_drop_database(AstDropDatabase && op)
{
    drop_operation_ = std::move(op);
}

void AstDropStatementNode::set_drop_collection(AstDropCollection && op)
{
    drop_operation_ = std::move(op);
}

void AstDropStatementNode::set_drop_index(AstDropIndex && op)
{
    drop_operation_ = std::move(op);
}

void AstDropStatementNode::set_drop_vindex(AstDropVIndex && op)
{
    drop_operation_ = std::move(op);
}

bool AstDropStatementNode::get_if_exists() const noexcept
{
    return if_exists_;
}

const AstDropDatabase & AstDropStatementNode::get_drop_database() const
{
    return std::get<AstDropDatabase>(drop_operation_);
}

const AstDropCollection & AstDropStatementNode::get_drop_collection() const
{
    return std::get<AstDropCollection>(drop_operation_);
}

const AstDropIndex & AstDropStatementNode::get_drop_index() const
{
    return std::get<AstDropIndex>(drop_operation_);
}

const AstDropVIndex & AstDropStatementNode::get_drop_vindex() const
{
    return std::get<AstDropVIndex>(drop_operation_);
}

bool AstDropStatementNode::has_drop_operation() const noexcept
{
    return !std::holds_alternative<std::monostate>(drop_operation_);
}

bool AstDropStatementNode::has_drop_database() const noexcept
{
    return std::holds_alternative<AstDropDatabase>(drop_operation_);
}

bool AstDropStatementNode::has_drop_collection() const noexcept
{
    return std::holds_alternative<AstDropCollection>(drop_operation_);
}

bool AstDropStatementNode::has_drop_index() const noexcept
{
    return std::holds_alternative<AstDropIndex>(drop_operation_);
}

bool AstDropStatementNode::has_drop_vindex() const noexcept
{
    return std::holds_alternative<AstDropVIndex>(drop_operation_);
}

} // namespace dreamdb
