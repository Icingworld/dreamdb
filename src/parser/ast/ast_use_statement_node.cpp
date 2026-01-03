#include "dreamdb/parser/ast/ast_use_statement_node.h"

namespace dreamdb
{

AstUseStatementNode::AstUseStatementNode(std::size_t line, std::size_t column)
    : AstStatementNode(AstStatementNodeType::AST_STATEMENT_USE, line, column)
    , database_name_(std::nullopt)
{
}

void AstUseStatementNode::set_database_name(const std::string & database_name)
{
    if (database_name.empty()) {
        database_name_ = std::nullopt;
    } else {
        database_name_ = database_name;
    }
}

const std::string & AstUseStatementNode::get_database_name() const
{
    return database_name_.value();
}

bool AstUseStatementNode::has_database_name() const noexcept
{
    return database_name_.has_value();
}

} // namespace dreamdb
