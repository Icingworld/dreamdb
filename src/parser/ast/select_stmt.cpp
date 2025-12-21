#include "dreamdb/parser/ast/select_stmt.h"

#include <sstream>

namespace dreamdb
{

SelectItem::SelectItem()
    : type(SelectItemType::STAR)
    , expression(nullptr)
    , alias("")
{
}

SelectItem SelectItem::create_star_item()
{
    SelectItem item;
    item.type = SelectItemType::STAR;
    return item;
}

SelectItem SelectItem::create_expression_item(std::unique_ptr<AstNode> expression, const std::string & alias)
{
    SelectItem item;
    item.type = SelectItemType::EXPRESSION;
    item.expression = std::move(expression);
    item.alias = alias;
    return item;
}

SelectStmt::SelectStmt(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::SELECT_STMT, line, column)
    , collection_name_("")
    , select_items_()
    , where_clause_(nullptr)
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

void SelectStmt::set_where_clause(std::unique_ptr<AstNode> expr)
{
    where_clause_ = std::move(expr);
}

void SelectStmt::set_limit(std::size_t limit)
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

        if (item.type == SelectItem::SelectItemType::STAR) {
            oss << "*";
        }
        else if (item.expression) {
            oss << item.expression->debug_string();
            if (!item.alias.empty()) {
                oss << " AS " << item.alias;
            }
        }
        else {
            oss << "<expr:null>";
        }
    }
    oss << "]";

    if (where_clause_) {
        oss << ", where=" << where_clause_->debug_string();
    }

    if (limit_.has_value()) {
        oss << ", limit=" << *limit_;
    }

    oss << ")";

    return oss.str();
}

} // namespace dreamdb
