#include "dreamdb/planner/physical/operator/physical_unary_operator.h"

namespace dreamdb::planner::physical
{

PhysicalUnaryOperator::PhysicalUnaryOperator(std::unique_ptr<PhysicalOperator> child)
    : child_(std::move(child))
{
}

void PhysicalUnaryOperator::open(dreamdb::executor::ExecutionContext & context)
{
    child_->open(context);
}

void PhysicalUnaryOperator::close(dreamdb::executor::ExecutionContext & context)
{
    child_->close(context);
}

} // namespace dreamdb::planner::physical
