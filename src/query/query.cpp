#include "dreamdb/query/query.h"

namespace dreamdb
{

Query & Query::where(const Condition & condition)
{
    condition_ = condition;
    return *this;
}

Query & Query::order_by(const Order & order)
{
    order_ = order;
    return *this;
}

Query & Query::limit(const Limit & limit)
{
    limit_ = limit;
    return *this;
}

const std::optional<Condition> & Query::get_condition() const noexcept
{
    return condition_;
}

const std::optional<Order> & Query::get_order() const noexcept
{
    return order_;
}

const std::optional<Limit> & Query::get_limit() const noexcept
{
    return limit_;
}

bool Query::has_condition() const noexcept
{
    return condition_.has_value();
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
    return !condition_.has_value() && !order_.has_value() && !limit_.has_value();
}

void Query::clear()
{
    condition_.reset();
    order_.reset();
    limit_.reset();
}

} // namespace dreamdb

