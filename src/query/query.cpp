#include "dreamdb/query/query.h"

namespace dreamdb
{

Query::Query() noexcept
    : where_clause_(nullptr)
    , order_(std::nullopt)
    , limit_(std::nullopt)
{
}

void Query::set_where_clause(const AstNode * where_clause) noexcept
{
    where_clause_ = where_clause;
}

void Query::set_order(const Order & order)
{
    order_ = order;
}

void Query::set_limit(const Limit & limit)
{
    limit_ = limit;
}

const AstNode * Query::get_where_clause() const noexcept
{
    return where_clause_;
}

const std::optional<Order> & Query::get_order() const noexcept
{
    return order_;
}

const std::optional<Limit> & Query::get_limit() const noexcept
{
    return limit_;
}

bool Query::has_where_clause() const noexcept
{
    return where_clause_ != nullptr;
}

bool Query::has_order() const noexcept
{
    return order_.has_value();
}

bool Query::has_limit() const noexcept
{
    return limit_.has_value();
}

bool Query::is_empty() const noexcept
{
    return where_clause_ == nullptr && !order_.has_value() && !limit_.has_value();
}

void Query::clear()
{
    where_clause_ = nullptr;
    order_.reset();
    limit_.reset();
}

} // namespace dreamdb
