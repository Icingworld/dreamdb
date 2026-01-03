#include "dreamdb/parser/ast/ast_delete_statement_node.h"

#include "dreamdb/parser/ast/ast_expression_node.h"

namespace dreamdb
{

AstDeleteStatementNode::AstDeleteStatementNode(std::size_t line, std::size_t column)
    : AstStatementNode(AstStatementNodeType::AST_STATEMENT_DELETE, line, column)
    , collection_name_(std::nullopt)
    , where_clause_(nullptr)
{
}

AstDeleteStatementNode::~AstDeleteStatementNode() noexcept = default;

void AstDeleteStatementNode::set_collection_name(const std::string & collection_name)
{
    if (collection_name.empty()) {
        collection_name_ = std::nullopt;
    } else {
        collection_name_ = collection_name;
    }
}

void AstDeleteStatementNode::set_where_clause(std::unique_ptr<AstExpressionNode> where_clause) noexcept
{
    where_clause_ = std::move(where_clause);
}

const std::string & AstDeleteStatementNode::get_collection_name() const
{
    return collection_name_.value();
}

const AstExpressionNode & AstDeleteStatementNode::get_where_clause() const
{
    return *where_clause_;
}

bool AstDeleteStatementNode::has_collection_name() const noexcept
{
    return collection_name_.has_value();
}

bool AstDeleteStatementNode::has_where_clause() const noexcept
{
    return where_clause_ != nullptr;
}

} // namespace dreamdb
