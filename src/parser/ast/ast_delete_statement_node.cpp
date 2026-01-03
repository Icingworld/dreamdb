#include "dreamdb/parser/ast/ast_delete_statement_node.h"

#include "dreamdb/parser/ast/ast_expression_node.h"

namespace dreamdb
{

AstDeleteStatementNode::AstDeleteStatementNode(std::size_t line, std::size_t column)
    : AstStatementNode(AstStatementNodeType::AST_STATEMENT_DELETE, line, column)
    , collection_name_(std::nullopt)
    , where_clause_(nullptr)
    , order_by_items_()
    , limit_(std::nullopt)
    , offset_(std::nullopt)
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

void AstDeleteStatementNode::add_order_by_item(OrderByItem && item) noexcept
{
    order_by_items_.push_back(std::move(item));
}
    
void AstDeleteStatementNode::set_limit(std::size_t limit) noexcept
{
    limit_ = limit;
}

void AstDeleteStatementNode::set_offset(std::size_t offset) noexcept
{
    offset_ = offset;
}

const std::string & AstDeleteStatementNode::get_collection_name() const
{
    return collection_name_.value();
}

const AstExpressionNode & AstDeleteStatementNode::get_where_clause() const
{
    return *where_clause_;
}

const std::vector<OrderByItem> & AstDeleteStatementNode::get_order_by_items() const noexcept
{
    return order_by_items_;
}

std::size_t AstDeleteStatementNode::get_limit() const
{
    return limit_.value();
}

std::size_t AstDeleteStatementNode::get_offset() const
{
    return offset_.value();
}

bool AstDeleteStatementNode::has_collection_name() const noexcept
{
    return collection_name_.has_value();
}

bool AstDeleteStatementNode::has_where_clause() const noexcept
{
    return where_clause_ != nullptr;
}

bool AstDeleteStatementNode::has_order_by_items() const noexcept
{
    return !order_by_items_.empty();
}

bool AstDeleteStatementNode::has_limit() const noexcept
{
    return limit_.has_value();
}

bool AstDeleteStatementNode::has_offset() const noexcept
{
    return offset_.has_value();
}

} // namespace dreamdb
