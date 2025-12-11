#include "dreamdb/query/order.h"

namespace dreamdb
{

Order::Order(std::uint8_t field_index, Direction direction) noexcept
    : field_index_(field_index)
    , direction_(direction)
{
}

std::uint8_t Order::get_field_index() const noexcept
{
    return field_index_;
}

Direction Order::get_direction() const noexcept
{
    return direction_;
}

} // namespace dreamdb
