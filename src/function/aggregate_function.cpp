#include "dreamdb/function/aggregate_function.h"

namespace dreamdb
{

AggregateFunction::AggregateFunction(std::string func_name, std::vector<LogicalType> arg_types)
    : name_(std::move(func_name)), argument_types_(std::move(arg_types))
{
}

const std::string & AggregateFunction::name() const noexcept
{
    return name_;
}

bool AggregateFunction::is_aggregate() const noexcept
{
    return true;
}

const std::vector<LogicalType> & AggregateFunction::argument_types() const noexcept
{
    return argument_types_;
}

std::optional<FieldValue> AggregateFunction::execute(const std::vector<FieldValue> & /*args*/) const
{
    return std::nullopt;
}

} // namespace dreamdb
