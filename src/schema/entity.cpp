#include "dreamdb/schema/entity.h"

#include <stdexcept>

namespace dreamdb
{

Entity::Entity(std::size_t field_count)
    : values(field_count, NullType())
{
}

void Entity::set_value(std::size_t index, const EntityValue & value)
{
    if (index >= values.size()) {
        throw std::out_of_range("Field index out of range");
    }

    values[index] = value;
}

const EntityValue & Entity::get_value(std::size_t index) const
{
    if (index >= values.size()) {
        throw std::out_of_range("Field index out of range");
    }

    return values[index];
}

EntityValue & Entity::get_value(std::size_t index)
{
    if (index >= values.size()) {
        throw std::out_of_range("Field index out of range");
    }

    return values[index];
}

bool Entity::is_null(std::size_t index) const
{
    if (index >= values.size()) {
        throw std::out_of_range("Field index out of range");
    }

    return std::holds_alternative<NullType>(values[index]);
}

std::size_t Entity::field_count() const
{
    return values.size();
}

void Entity::clear()
{
    for (auto & value : values) {
        value = NullType();
    }
}

bool Entity::is_empty() const
{
    for (const auto & value : values) {
        if (!std::holds_alternative<NullType>(value)) {
            return false;
        }
    }

    return true;
}

} // namespace dreamdb
