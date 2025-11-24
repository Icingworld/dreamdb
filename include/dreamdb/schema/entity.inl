#pragma once

#include <stdexcept>

#include "dreamdb/schema/entity.h"

namespace dreamdb
{

template <typename T>
T Entity::get_value_as(std::size_t index) const
{
    if (index >= values.size()) {
        throw std::out_of_range("Field index out of range");
    }

    return std::get<T>(values[index]);
}

} // namespace dreamdb
