#include "dreamdb/parser/ast/select_stmt.h"

#include <sstream>

namespace dreamdb
{

SelectItem::SelectItem()
    : select_item_type_(SelectItemType::STAR)
    , select_item_expression_(nullptr)
    , select_item_alias_("")
{
}

SelectItem SelectItem::create_star_item()
{
    SelectItem item;
    item.select_item_type_ = SelectItemType::STAR;
    return item;
}

SelectItem SelectItem::create_expression_item(std::unique_ptr<AstNode> expression, const std::string & alias)
{
    SelectItem item;
    item.select_item_type_ = SelectItemType::EXPRESSION;
    item.select_item_expression_ = std::move(expression);
    item.select_item_alias_ = alias;
    return item;
}

SelectItem::SelectItemType SelectItem::get_select_item_type() const noexcept
{
    return select_item_type_;
}

const AstNode * SelectItem::get_select_item_expression() const noexcept
{
    return select_item_expression_.get();
}

const std::string & SelectItem::get_select_item_alias() const noexcept
{
    return select_item_alias_;
}

OrderByItem::OrderByItem()
    : expression_(nullptr)
    , order_type_(Direction::ASC)
{
}

void OrderByItem::set_expression(std::unique_ptr<AstNode> expression) noexcept
{
    expression_ = std::move(expression);
}

void OrderByItem::set_order_type(Direction order_type) noexcept
{
    order_type_ = order_type;
}

const AstNode * OrderByItem::get_expression() const noexcept
{
    return expression_.get();
}

Direction OrderByItem::get_order_type() const noexcept
{
    return order_type_;
}

SelectStmt::SelectStmt(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::SELECT_STMT, line, column)
    , collection_name_("")
    , select_items_()
    , where_clause_(nullptr)
    , group_by_exprs_()
    , having_clause_(nullptr)
    , order_by_items_()
    , limit_(std::nullopt)
{
}

void SelectStmt::set_collection_name(const std::string & collection_name)
{
    collection_name_ = collection_name;
}

void SelectStmt::add_select_item(SelectItem && item)
{
    select_items_.push_back(std::move(item));
}

void SelectStmt::set_where_clause(std::unique_ptr<AstNode> expr) noexcept
{
    where_clause_ = std::move(expr);
}

void SelectStmt::add_group_by_expression(std::unique_ptr<AstNode> expression) noexcept
{
    group_by_exprs_.push_back(std::move(expression));
}

void SelectStmt::set_having_clause(std::unique_ptr<AstNode> expr) noexcept
{
    having_clause_ = std::move(expr);
}

void SelectStmt::add_order_by_item(OrderByItem && item) noexcept
{
    order_by_items_.push_back(std::move(item));
}

void SelectStmt::set_limit(std::size_t limit) noexcept
{
    limit_ = limit;
}

const std::string & SelectStmt::get_collection_name() const noexcept
{
    return collection_name_;
}

const std::vector<SelectItem> & SelectStmt::get_select_items() const noexcept
{
    return select_items_;
}

const AstNode * SelectStmt::get_where_clause() const noexcept
{
    return where_clause_.get();
}

const std::vector<std::unique_ptr<AstNode>> & SelectStmt::get_group_by_expressions() const noexcept
{
    return group_by_exprs_;
}

const AstNode * SelectStmt::get_having_clause() const noexcept
{
    return having_clause_.get();
}

const std::vector<OrderByItem> & SelectStmt::get_order_by_items() const noexcept
{
    return order_by_items_;
}

std::optional<std::size_t> SelectStmt::get_limit() const noexcept
{
    return limit_;
}

std::string SelectStmt::debug_string() const
{
    std::ostringstream oss;
    oss << "SelectStmt(collection_name=" << (collection_name_.empty() ? "<none>" : collection_name_)
        << ", columns=[";

    for (std::size_t i = 0; i < select_items_.size(); ++i) {
        const auto & item = select_items_[i];
        if (i > 0) {
            oss << ", ";
        }

        if (item.get_select_item_type() == SelectItem::SelectItemType::STAR) {
            oss << "*";
        }
        else if (item.get_select_item_expression()) {
            oss << item.get_select_item_expression()->debug_string();
            if (!item.get_select_item_alias().empty()) {
                oss << " AS " << item.get_select_item_alias();
            }
        }
        else {
            oss << "<expr:null>";
        }
    }
    oss << "]";

    if (where_clause_ != nullptr) {
        oss << ", where_clause=" << where_clause_->debug_string();
    } else {
        oss << ", where_clause=<none>";
    }

    if (!group_by_exprs_.empty()) {
        oss << ", group_by=[";
        for (std::size_t i = 0; i < group_by_exprs_.size(); ++i) {
            if (i > 0) {
                oss << ", ";
            }
            if (group_by_exprs_[i]) {
                oss << group_by_exprs_[i]->debug_string();
            } else {
                oss << "<null>";
            }
        }
        oss << "]";
    } else {
        oss << ", group_by=<none>";
    }

    if (having_clause_ != nullptr) {
        oss << ", having_clause=" << having_clause_->debug_string();
    } else {
        oss << ", having_clause=<none>";
    }

    if (!order_by_items_.empty()) {
        oss << ", order_by=[";
        for (std::size_t i = 0; i < order_by_items_.size(); ++i) {
            if (i > 0) {
                oss << ", ";
            }
            const auto & item = order_by_items_[i];
            if (item.get_expression()) {
                oss << item.get_expression()->debug_string();
            } else {
                oss << "<null>";
            }
            oss << " " << (item.get_order_type() == Direction::ASC ? "ASC" : "DESC");
        }
        oss << "]";
    } else {
        oss << ", order_by=<none>";
    }

    if (limit_ != std::nullopt) {
        oss << ", limit=" << limit_.value();
    } else {
        oss << ", limit=<none>";
    }

    oss << ")";

    return oss.str();
}

} // namespace dreamdb
