#include "dreamdb/parser/ast/order_by_item.h"

namespace dreamdb
{

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

} // namespace dreamdb
