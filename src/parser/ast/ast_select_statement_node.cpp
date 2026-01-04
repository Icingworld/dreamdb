#include "dreamdb/parser/ast/ast_select_statement_node.h"

#include "dreamdb/parser/ast/ast_expression_node.h"

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
    item.select_item_alias_ = alias;
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

OrderByItem::OrderByItem(std::unique_ptr<AstExpressionNode> expression, Direction order_type)
    : expression_(std::move(expression))
    , order_type_(order_type)
{
}

OrderByItem::~OrderByItem() noexcept = default;

const AstExpressionNode & OrderByItem::get_expression() const noexcept
{
    return *expression_;
}

Direction OrderByItem::get_order_type() const noexcept
{
    return order_type_;
}

bool OrderByItem::has_expression() const noexcept
{
    return expression_ != nullptr;
}

AstSelectStatementNode::AstSelectStatementNode(std::size_t line, std::size_t column)
    : AstStatementNode(AstStatementNodeType::AST_STATEMENT_SELECT, line, column)
    , collection_name_(std::nullopt)
    , select_items_()
    , where_clause_(nullptr)
    , group_by_clauses_()
    , having_clause_(nullptr)
    , order_by_items_()
    , limit_(nullptr)
    , offset_(nullptr)
{
}

AstSelectStatementNode::~AstSelectStatementNode() noexcept = default;

void AstSelectStatementNode::set_collection_name(const std::string & collection_name)
{
    collection_name_ = collection_name;
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

void AstSelectStatementNode::set_limit(std::unique_ptr<AstExpressionNode> limit) noexcept
{
    limit_ = std::move(limit);
}

void AstSelectStatementNode::set_offset(std::unique_ptr<AstExpressionNode> offset) noexcept
{
    offset_ = std::move(offset);
}

const std::string & AstSelectStatementNode::get_collection_name() const
{
    return collection_name_.value();
}

const std::vector<SelectItem> & AstSelectStatementNode::get_select_items() const noexcept
{
    return select_items_;
}

const AstExpressionNode & AstSelectStatementNode::get_where_clause() const
{
    return *where_clause_;
}

const std::vector<std::unique_ptr<AstExpressionNode>> & AstSelectStatementNode::get_group_by_clauses() const noexcept
{
    return group_by_clauses_;
}

const AstExpressionNode & AstSelectStatementNode::get_having_clause() const
{
    return *having_clause_;
}

const std::vector<OrderByItem> & AstSelectStatementNode::get_order_by_items() const noexcept
{
    return order_by_items_;
}

const AstExpressionNode & AstSelectStatementNode::get_limit() const
{
    return *limit_;
}

const AstExpressionNode & AstSelectStatementNode::get_offset() const
{
    return *offset_;
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
    return limit_ != nullptr;
}

bool AstSelectStatementNode::has_offset() const noexcept
{
    return offset_ != nullptr;
}

} // namespace dreamdb
