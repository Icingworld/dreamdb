#include "dreamdb/planner/physical/operator/physical_unary_operator.h"

namespace dreamdb::planner::physical
{

PhysicalUnaryOperator::PhysicalUnaryOperator(std::unique_ptr<PhysicalOperator> child)
    : child_(std::move(child))
{
}

void PhysicalUnaryOperator::open(ExecutionContext & context)
{
    child_->open(context);
}

void PhysicalUnaryOperator::close(ExecutionContext & context)
{
    child_->close(context);
}

} // namespace dreamdb::planner::physical
