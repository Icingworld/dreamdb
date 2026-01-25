#include "dreamdb/planner/logical/logical_operator.h"

#include <cassert>

namespace dreamdb::planner::logical
{

LogicalOperator::LogicalOperator(LogicalOperatorType operator_type) noexcept
    : operator_type_(operator_type)
{
}

LogicalOperatorType LogicalOperator::operator_type() const noexcept
{
    return operator_type_;
}

const LogicalOutputSchema & LogicalOperator::output_schema() const noexcept
{
    return output_schema_;
}

void LogicalOperator::add_child(std::unique_ptr<LogicalOperator> child)
{
    children_.push_back(std::move(child));
}

std::size_t LogicalOperator::child_count() const noexcept
{
    return children_.size();
}

const LogicalOperator & LogicalOperator::child_at(std::size_t index) const noexcept
{
    // 索引不能超过子节点数量
    assert(index < children_.size());

    return *children_[index];
}

} // namespace dreamdb::planner::logical
