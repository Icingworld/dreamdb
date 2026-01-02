#include "dreamdb/parser/ast/ast_alter_statement_node.h"

namespace dreamdb
{

AstAlterStatementNode::AstAlterStatementNode(std::size_t line, std::size_t column)
    : AstStatementNode(AstStatementNodeType::AST_STATEMENT_ALTER, line, column)
    , collection_name_(std::nullopt)
    , alter_type_(AstAlterType::AST_ALTER_UNKNOWN)
    , alter_operation_(std::monostate())
{
}

void AstAlterStatementNode::set_collection_name(const std::string & collection_name)
{
    if (collection_name.empty()) {
        collection_name_ = std::nullopt;
    } else {
        collection_name_ = collection_name;
    }
}

void AstAlterStatementNode::set_alter_type(AstAlterType alter_type) noexcept
{
    alter_type_ = alter_type;
}

void AstAlterStatementNode::set_add_column(AstAlterAddColumn && op) noexcept
{
    alter_operation_ = std::move(op);
}

void AstAlterStatementNode::set_drop_column(AstAlterDropColumn && op) noexcept
{
    alter_operation_ = std::move(op);
}

void AstAlterStatementNode::set_modify_column(AstAlterModifyColumn && op) noexcept
{
    alter_operation_ = std::move(op);
}

void AstAlterStatementNode::set_rename_column(AstAlterRenameColumn && op) noexcept
{
    alter_operation_ = std::move(op);
}

const std::string & AstAlterStatementNode::get_collection_name() const noexcept
{
    return collection_name_.value();
}

AstAlterType AstAlterStatementNode::get_alter_type() const noexcept
{
    return alter_type_;
}

const AstAlterAddColumn & AstAlterStatementNode::get_add_column() const
{
    return std::get<AstAlterAddColumn>(alter_operation_);
}

const AstAlterDropColumn & AstAlterStatementNode::get_drop_column() const
{
    return std::get<AstAlterDropColumn>(alter_operation_);
}

const AstAlterModifyColumn & AstAlterStatementNode::get_modify_column() const
{
    return std::get<AstAlterModifyColumn>(alter_operation_);
}

const AstAlterRenameColumn & AstAlterStatementNode::get_rename_column() const
{
    return std::get<AstAlterRenameColumn>(alter_operation_);
}

bool AstAlterStatementNode::has_collection_name() const noexcept
{
    return collection_name_.has_value();
}

bool AstAlterStatementNode::has_alter_operation() const noexcept
{
    return !std::holds_alternative<std::monostate>(alter_operation_);
}

} // namespace dreamdb
