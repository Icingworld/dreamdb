#include "dreamdb/parser/ast/ast_select_statement_node.h"

namespace dreamdb
{

SelectItem::SelectItem()
    : select_item_type_(SelectItemType::SELECT_ITEM_STAR)
    , select_item_expression_(nullptr)
    , select_item_alias_(std::nullopt)
{
}

SelectItem SelectItem::create_star_item()
{
    SelectItem item;
    item.select_item_type_ = SelectItemType::SELECT_ITEM_STAR;
    return item;
}

SelectItem SelectItem::create_expression_item(std::unique_ptr<AstExpressionNode> expression, const std::string & alias)
{
    SelectItem item;
    item.select_item_type_ = SelectItemType::SELECT_ITEM_EXPRESSION;
    item.select_item_expression_ = std::move(expression);
    if (alias.empty()) {
        item.select_item_alias_ = std::nullopt;
    } else {
        item.select_item_alias_ = alias;
    }
    return item;
}

SelectItemType SelectItem::get_select_item_type() const noexcept
{
    return select_item_type_;
}

const AstExpressionNode & SelectItem::get_select_item_expression() const noexcept
{
    return *select_item_expression_;
}

const std::string & SelectItem::get_select_item_alias() const noexcept
{
    return select_item_alias_.value();
}

bool SelectItem::has_select_item_expression() const noexcept
{
    return select_item_expression_ != nullptr;
}

bool SelectItem::has_select_item_alias() const noexcept
{
    return select_item_alias_.has_value();
}

AstSelectStatementNode::AstSelectStatementNode(std::size_t line, std::size_t column)
    : AstStatementNode(AstStatementNodeType::AST_STATEMENT_SELECT, line, column)
    , collection_name_(std::nullopt)
    , select_items_()
    , where_clause_(nullptr)
    , group_by_clauses_()
    , having_clause_(nullptr)
    , order_by_items_()
    , limit_(std::nullopt)
{
}

AstSelectStatementNode::~AstSelectStatementNode() noexcept = default;

void AstSelectStatementNode::set_collection_name(const std::string & collection_name)
{
    if (collection_name.empty()) {
        collection_name_ = std::nullopt;
    } else {
        collection_name_ = collection_name;
    }
}

void AstSelectStatementNode::add_select_item(SelectItem && item)
{
    select_items_.push_back(std::move(item));
}

void AstSelectStatementNode::set_where_clause(std::unique_ptr<AstExpressionNode> expr) noexcept
{
    where_clause_ = std::move(expr);
}

void AstSelectStatementNode::add_group_by_clause(std::unique_ptr<AstExpressionNode> group_by_clause) noexcept
{
    group_by_clauses_.push_back(std::move(group_by_clause));
}

void AstSelectStatementNode::set_having_clause(std::unique_ptr<AstExpressionNode> expr) noexcept
{
    having_clause_ = std::move(expr);
}

void AstSelectStatementNode::add_order_by_item(OrderByItem && item) noexcept
{
    order_by_items_.push_back(std::move(item));
}

void AstSelectStatementNode::set_limit(std::size_t limit) noexcept
{
    limit_ = limit;
}

void AstSelectStatementNode::set_offset(std::size_t offset) noexcept
{
    offset_ = offset;
}

const std::string & AstSelectStatementNode::get_collection_name() const noexcept
{
    return collection_name_.value();
}

const std::vector<SelectItem> & AstSelectStatementNode::get_select_items() const noexcept
{
    return select_items_;
}

const AstExpressionNode & AstSelectStatementNode::get_where_clause() const noexcept
{
    return *where_clause_;
}

const std::vector<std::unique_ptr<AstExpressionNode>> & AstSelectStatementNode::get_group_by_clauses() const noexcept
{
    return group_by_clauses_;
}

const AstExpressionNode & AstSelectStatementNode::get_having_clause() const noexcept
{
    return *having_clause_;
}

const std::vector<OrderByItem> & AstSelectStatementNode::get_order_by_items() const noexcept
{
    return order_by_items_;
}

std::size_t AstSelectStatementNode::get_limit() const noexcept
{
    return limit_.value();
}

std::size_t AstSelectStatementNode::get_offset() const noexcept
{
    return offset_.value();
}

bool AstSelectStatementNode::has_collection_name() const noexcept
{
    return collection_name_.has_value();
}

bool AstSelectStatementNode::has_select_items() const noexcept
{
    return !select_items_.empty();
}

bool AstSelectStatementNode::has_where_clause() const noexcept
{
    return where_clause_ != nullptr;
}

bool AstSelectStatementNode::has_group_by_clauses() const noexcept
{
    return !group_by_clauses_.empty();
}

bool AstSelectStatementNode::has_having_clause() const noexcept
{
    return having_clause_ != nullptr;
}

bool AstSelectStatementNode::has_order_by_items() const noexcept
{
    return !order_by_items_.empty();
}

bool AstSelectStatementNode::has_limit() const noexcept
{
    return limit_.has_value();
}

bool AstSelectStatementNode::has_offset() const noexcept
{
    return offset_.has_value();
}

} // namespace dreamdb
