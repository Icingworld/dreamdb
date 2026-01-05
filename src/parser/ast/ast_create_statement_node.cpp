#include "dreamdb/parser/ast/ast_create_statement_node.h"

namespace dreamdb
{

AstVIndexWithOption::AstVIndexWithOption(const std::string & key, std::unique_ptr<AstExpressionNode> value)
    : key_(key)
    , value_(std::move(value))
{
}

const std::string & AstVIndexWithOption::get_key() const noexcept
{
    return key_;
}

const std::unique_ptr<AstExpressionNode> & AstVIndexWithOption::get_value() const noexcept
{
    return value_;
}

bool AstVIndexWithOption::has_value() const noexcept
{
    return value_ != nullptr;
}

AstCreateDatabase::AstCreateDatabase(const std::string & database_name)
    : database_name_(database_name)
{
}

const std::string & AstCreateDatabase::get_database_name() const noexcept
{
    return database_name_;
}

AstCreateCollection::AstCreateCollection(const std::string & collection_name, std::vector<AstColumnDefinition> && column_definitions)
    : collection_name_(collection_name)
    , column_definitions_(std::move(column_definitions))
{
}

const std::string & AstCreateCollection::get_collection_name() const noexcept
{
    return collection_name_;
}

const std::vector<AstColumnDefinition> & AstCreateCollection::get_column_definitions() const noexcept
{
    return column_definitions_;
}

AstCreateIndex::AstCreateIndex(const std::string & index_name, const std::string & collection_name, const std::vector<std::string> & column_names, const std::string & index_type)
    : index_name_(index_name)
    , collection_name_(collection_name)
    , column_names_(column_names)
    , index_type_(index_type)
{
}

const std::string & AstCreateIndex::get_index_name() const noexcept
{
    return index_name_;
}

const std::string & AstCreateIndex::get_collection_name() const noexcept
{
    return collection_name_;
}

const std::vector<std::string> & AstCreateIndex::get_column_names() const noexcept
{
    return column_names_;
}

const std::string & AstCreateIndex::get_index_type() const noexcept
{
    return index_type_;
}

AstCreateVIndex::AstCreateVIndex(const std::string & vindex_name, const std::string & collection_name, const std::string & column_name, const std::string & vindex_type, std::vector<AstVIndexWithOption> with_clauses)
    : vindex_name_(vindex_name)
    , collection_name_(collection_name)
    , column_name_(column_name)
    , vindex_type_(vindex_type)
    , with_clauses_(std::move(with_clauses))
{
}

const std::string & AstCreateVIndex::get_vindex_name() const noexcept
{
    return vindex_name_;
}

const std::string & AstCreateVIndex::get_collection_name() const noexcept
{
    return collection_name_;
}

const std::string & AstCreateVIndex::get_column_name() const noexcept
{
    return column_name_;
}

const std::string & AstCreateVIndex::get_vindex_type() const noexcept
{
    return vindex_type_;
}

const std::vector<AstVIndexWithOption> & AstCreateVIndex::get_with_clauses() const noexcept
{
    return with_clauses_;
}

AstCreateStatementNode::AstCreateStatementNode(std::size_t line, std::size_t column)
    : AstStatementNode(AstStatementNodeType::AST_STATEMENT_CREATE, line, column)
    , if_not_exists_(false)
    , create_operation_(std::monostate())
{
}

void AstCreateStatementNode::set_if_not_exists(bool if_not_exists) noexcept
{
    if_not_exists_ = if_not_exists;
}

void AstCreateStatementNode::set_create_database(AstCreateDatabase && op)
{
    create_operation_ = std::move(op);
}

void AstCreateStatementNode::set_create_collection(AstCreateCollection && op)
{
    create_operation_ = std::move(op);
}

void AstCreateStatementNode::set_create_index(AstCreateIndex && op)
{
    create_operation_ = std::move(op);
}

void AstCreateStatementNode::set_create_vindex(AstCreateVIndex && op)
{
    create_operation_ = std::move(op);
}

bool AstCreateStatementNode::get_if_not_exists() const noexcept
{
    return if_not_exists_;
}

const AstCreateDatabase & AstCreateStatementNode::get_create_database() const
{
    return std::get<AstCreateDatabase>(create_operation_);
}

const AstCreateCollection & AstCreateStatementNode::get_create_collection() const
{
    return std::get<AstCreateCollection>(create_operation_);
}

const AstCreateIndex & AstCreateStatementNode::get_create_index() const
{
    return std::get<AstCreateIndex>(create_operation_);
}

const AstCreateVIndex & AstCreateStatementNode::get_create_vindex() const
{
    return std::get<AstCreateVIndex>(create_operation_);
}

bool AstCreateStatementNode::has_create_operation() const noexcept
{
    return !std::holds_alternative<std::monostate>(create_operation_);
}

bool AstCreateStatementNode::has_create_database() const noexcept
{
    return std::holds_alternative<AstCreateDatabase>(create_operation_);
}

bool AstCreateStatementNode::has_create_collection() const noexcept
{
    return std::holds_alternative<AstCreateCollection>(create_operation_);
}

bool AstCreateStatementNode::has_create_index() const noexcept
{
    return std::holds_alternative<AstCreateIndex>(create_operation_);
}

bool AstCreateStatementNode::has_create_vindex() const noexcept
{
    return std::holds_alternative<AstCreateVIndex>(create_operation_);
}

} // namespace dreamdb
