#include "dreamdb/function/scalar_function.h"

namespace dreamdb
{

ScalarFunction::ScalarFunction(const std::string & name, const std::vector<LogicalType> & argument_types) noexcept
    : name_(name)
    , argument_types_(argument_types)
{
}

const std::string & ScalarFunction::name() const noexcept
{
    return name_;
}

bool ScalarFunction::is_aggregate() const noexcept
{
    return false;
}

const std::vector<LogicalType> & ScalarFunction::argument_types() const noexcept
{
    return argument_types_;
}

} // namespace dreamdb
