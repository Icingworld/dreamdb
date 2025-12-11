#include "dreamdb/schema/entity.h"

#include <stdexcept>

#include "dreamdb/common/null.h"

namespace dreamdb
{

Entity::Entity(std::int64_t id, std::size_t field_count)
    : id_(id)
    , values_(field_count, Null())
{
}

void Entity::set_value(std::size_t index, const FieldValue & value)
{
    if (index >= values_.size()) {
        throw std::out_of_range("Field index out of range");
    }

    values_[index] = value;
}

std::int64_t Entity::get_id() const
{
    return id_;
}

const FieldValue & Entity::get_value(std::size_t index) const
{
    if (index >= values_.size()) {
        throw std::out_of_range("Field index out of range");
    }

    return values_[index];
}

FieldValue & Entity::get_value(std::size_t index)
{
    if (index >= values_.size()) {
        throw std::out_of_range("Field index out of range");
    }

    return values_[index];
}

bool Entity::is_null(std::size_t index) const
{
    if (index >= values_.size()) {
        throw std::out_of_range("Field index out of range");
    }

    return std::holds_alternative<Null>(values_[index]);
}

std::size_t Entity::field_count() const
{
    return values_.size();
}

void Entity::clear()
{
    for (auto & value : values_) {
        value = Null();
    }
}

bool Entity::is_empty() const
{
    for (const auto & value : values_) {
        if (!std::holds_alternative<Null>(value)) {
            return false;
        }
    }

    return true;
}

} // namespace dreamdb
