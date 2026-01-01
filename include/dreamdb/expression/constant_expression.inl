#pragma once

#include "dreamdb/expression/constant_expression.h"

namespace dreamdb
{

template <typename T>
bool ConstantExpression::is_type() const noexcept
{
    return std::holds_alternative<T>(field_value_);
}

template <typename T>
const T * ConstantExpression::get_field_value_pointer_if_as() const noexcept
{
    return std::get_if<T>(&field_value_);
}

template <typename T>
const T & ConstantExpression::get_field_value_as() const
{
    return std::get<T>(field_value_);
}

} // namespace dreamdb
