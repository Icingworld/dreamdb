#include "dreamdb/parser/ast/ast_show_statement_node.h"

namespace dreamdb
{

AstShowStatementNode::AstShowStatementNode(std::size_t line, std::size_t column)
    : AstStatementNode(AstStatementNodeType::AST_STATEMENT_SHOW, line, column)
    , show_type_(AstShowType::AST_SHOW_UNKNOWN)
    , collection_name_(std::nullopt)
{
}

void AstShowStatementNode::set_show_type(AstShowType show_type) noexcept
{
    show_type_ = show_type;
}

void AstShowStatementNode::set_collection_name(const std::string & collection_name)
{
    if (collection_name.empty()) {
        collection_name_ = std::nullopt;
    } else {
        collection_name_ = collection_name;
    }
}

AstShowType AstShowStatementNode::get_show_type() const noexcept
{
    return show_type_;
}

const std::string & AstShowStatementNode::get_collection_name() const
{
    return collection_name_.value();
}

bool AstShowStatementNode::has_collection_name() const noexcept
{
    return collection_name_.has_value();
}

} // namespace dreamdb
