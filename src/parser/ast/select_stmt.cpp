#include "dreamdb/parser/ast/select_stmt.h"

#include <sstream>

namespace dreamdb
{

SelectItem::SelectItem()
    : type(Type::STAR)
    , expression(nullptr)
    , alias("")
{
}

SelectItem SelectItem::create_star()
{
    SelectItem item;
    item.type = Type::STAR;
    return item;
}

SelectItem SelectItem::create_expression(std::unique_ptr<AstNode> expr, const std::string & alias)
{
    SelectItem item;
    item.type = Type::EXPRESSION;
    item.expression = std::move(expr);
    item.alias = alias;
    return item;
}

SelectStmt::SelectStmt(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::SELECT_STMT, line, column)
{
}

void SelectStmt::set_table_name(const std::string & table)
{
    table_name = table;
}

const std::string & SelectStmt::get_table_name() const noexcept
{
    return table_name;
}

void SelectStmt::add_select_item(SelectItem item)
{
    select_items.push_back(std::move(item));
}

const std::vector<SelectItem> & SelectStmt::get_select_items() const noexcept
{
    return select_items;
}

void SelectStmt::set_where_clause(std::unique_ptr<AstNode> expr)
{
    where_clause = std::move(expr);
}

const AstNode * SelectStmt::get_where_clause() const noexcept
{
    return where_clause.get();
}

void SelectStmt::set_limit(std::size_t limit)
{
    limit = limit;
}

std::optional<std::size_t> SelectStmt::get_limit() const noexcept
{
    return limit;
}

std::string SelectStmt::debug_string() const
{
    std::ostringstream oss;
    oss << "SelectStmt(table=" << (table_name.empty() ? "<none>" : table_name)
        << ", columns=[";

    for (std::size_t i = 0; i < select_items.size(); ++i) {
        const auto & item = select_items[i];
        if (i > 0) {
            oss << ", ";
        }

        if (item.type == SelectItem::Type::STAR) {
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

    if (where_clause) {
        oss << ", where=" << where_clause->debug_string();
    }

    if (limit.has_value()) {
        oss << ", limit=" << *limit;
    }

    oss << ")";
    return oss.str();
}

} // namespace dreamdb

