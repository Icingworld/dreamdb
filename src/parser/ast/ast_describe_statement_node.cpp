#include "dreamdb/parser/ast/ast_describe_statement_node.h"

namespace dreamdb
{

AstDescribeStatementNode::AstDescribeStatementNode(std::size_t line, std::size_t column)
    : AstStatementNode(AstStatementNodeType::AST_STATEMENT_DESCRIBE, line, column)
    , collection_name_(std::nullopt)
{
}

void AstDescribeStatementNode::set_collection_name(const std::string & collection_name)
{
    if (collection_name.empty()) {
        collection_name_ = std::nullopt;
    } else {
        collection_name_ = collection_name;
    }
}

const std::string & AstDescribeStatementNode::get_collection_name() const noexcept
{
    return collection_name_.value();
}

bool AstDescribeStatementNode::has_collection_name() const noexcept
{
    return collection_name_.has_value();
}

} // namespace dreamdb
