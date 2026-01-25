#include "dreamdb/binder/bound/expression/function_call.h"

#include <cassert>

#include "dreamdb/binder/bound/expression/visitor.h"

namespace dreamdb::binder::bound
{

BoundFunctionCallExpression::BoundFunctionCallExpression(
    std::string function_name,
    std::vector<std::unique_ptr<BoundExpression>> arguments,
    dreamdb::common::LogicalType logical_type,
    bool is_aggregate
)
    : BoundExpression(BoundExpressionType::FunctionCall, logical_type)
    , function_name_(std::move(function_name))
    , arguments_(std::move(arguments))
    , is_aggregate_(is_aggregate)
{
}

const std::string & BoundFunctionCallExpression::function_name() const noexcept
{
    return function_name_;
}

std::size_t BoundFunctionCallExpression::argument_count() const noexcept
{
    return arguments_.size();
}

const BoundExpression & BoundFunctionCallExpression::argument_at(std::size_t index) const noexcept
{
    assert(index < arguments_.size());
    assert(arguments_[index] != nullptr);
    return *arguments_[index];
}

bool BoundFunctionCallExpression::is_aggregate() const noexcept
{
    return is_aggregate_;
}

void BoundFunctionCallExpression::accept(BoundExpressionVisitor & visitor) const
{
    visitor.visit(*this);
}

std::unique_ptr<BoundExpression> BoundFunctionCallExpression::clone() const
{
    std::vector<std::unique_ptr<BoundExpression>> cloned_arguments;
    cloned_arguments.reserve(argument_count());
    for (std::size_t i = 0; i < argument_count(); ++i) {
        cloned_arguments.push_back(argument_at(i).clone());
    }

    return std::make_unique<BoundFunctionCallExpression>(
        function_name(),
        std::move(cloned_arguments),
        logical_type(),
        is_aggregate()
    );
}

} // namespace dreamdb::binder::bound
