#include "dreamdb/parser/ast/ast_drop_statement_node.h"

namespace dreamdb
{

AstDropStatementNode::AstDropStatementNode(std::size_t line, std::size_t column)
    : AstStatementNode(AstStatementNodeType::AST_STATEMENT_DROP, line, column)
    , drop_type_(AstDropType::AST_DROP_UNKNOWN)
    , object_name_(std::nullopt)
    , collection_name_(std::nullopt)
{
}

void AstDropStatementNode::set_drop_type(AstDropType drop_type) noexcept
{
    drop_type_ = drop_type;
}

void AstDropStatementNode::set_object_name(const std::string & object_name)
{
    if (object_name.empty()) {
        object_name_ = std::nullopt;
    } else {
        object_name_ = object_name;
    }
}

void AstDropStatementNode::set_collection_name(const std::string & collection_name)
{
    if (collection_name.empty()) {
        collection_name_ = std::nullopt;
    } else {
        collection_name_ = collection_name;
    }
}

AstDropType AstDropStatementNode::get_drop_type() const noexcept
{
    return drop_type_;
}

const std::string & AstDropStatementNode::get_object_name() const
{
    return object_name_.value();
}

const std::string & AstDropStatementNode::get_collection_name() const
{
    return collection_name_.value();
}

bool AstDropStatementNode::has_object_name() const noexcept
{
    return object_name_.has_value();
}

bool AstDropStatementNode::has_collection_name() const noexcept
{
    return collection_name_.has_value();
}

} // namespace dreamdb
