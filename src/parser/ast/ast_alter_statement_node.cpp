#include "dreamdb/parser/ast/ast_alter_statement_node.h"

namespace dreamdb
{

AstAlterAddColumn::AstAlterAddColumn(ColumnDefinition && column)
    : column_(std::move(column))
{
}

const ColumnDefinition & AstAlterAddColumn::get_column() const noexcept
{
    return column_;
}

AstAlterDropColumn::AstAlterDropColumn(const std::string & column_name)
    : column_name_(column_name)
{
}

const std::string & AstAlterDropColumn::get_column_name() const noexcept
{
    return column_name_;
}

AstAlterModifyColumn::AstAlterModifyColumn(const std::string & column_name, ColumnDefinition && new_definition)
    : column_name_(column_name)
    , new_definition_(std::move(new_definition))
{
}

const std::string & AstAlterModifyColumn::get_column_name() const noexcept
{
    return column_name_;
}

const ColumnDefinition & AstAlterModifyColumn::get_new_definition() const noexcept
{
    return new_definition_;
}

AstAlterRenameColumn::AstAlterRenameColumn(const std::string & old_name, const std::string & new_name)
    : old_name_(old_name)
    , new_name_(new_name)
{
}

const std::string & AstAlterRenameColumn::get_old_name() const noexcept
{
    return old_name_;
}

const std::string & AstAlterRenameColumn::get_new_name() const noexcept
{
    return new_name_;
}

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

void AstAlterStatementNode::set_alter_type(AstAlterType alter_type)
{
    alter_type_ = alter_type;
}

void AstAlterStatementNode::set_add_column(AstAlterAddColumn && op)
{
    alter_operation_ = std::move(op);
}

void AstAlterStatementNode::set_drop_column(AstAlterDropColumn && op)
{
    alter_operation_ = std::move(op);
}

void AstAlterStatementNode::set_modify_column(AstAlterModifyColumn && op)
{
    alter_operation_ = std::move(op);
}

void AstAlterStatementNode::set_rename_column(AstAlterRenameColumn && op)
{
    alter_operation_ = std::move(op);
}

const std::string & AstAlterStatementNode::get_collection_name() const
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

bool AstAlterStatementNode::has_add_column() const noexcept
{
    return std::holds_alternative<AstAlterAddColumn>(alter_operation_);
}

bool AstAlterStatementNode::has_drop_column() const noexcept
{
    return std::holds_alternative<AstAlterDropColumn>(alter_operation_);
}

bool AstAlterStatementNode::has_modify_column() const noexcept
{
    return std::holds_alternative<AstAlterModifyColumn>(alter_operation_);
}

bool AstAlterStatementNode::has_rename_column() const noexcept
{
    return std::holds_alternative<AstAlterRenameColumn>(alter_operation_);
}

} // namespace dreamdb
