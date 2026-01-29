#include "dreamdb/planner/physical/operator/physical_filter.h"

namespace dreamdb::planner::physical
{

PhysicalFilter::PhysicalFilter(
    std::unique_ptr<PhysicalOperator> child,
    std::unique_ptr<dreamdb::binder::bound::BoundExpression> predicate
)
    : PhysicalUnaryOperator(std::move(child))
    , predicate_(std::move(predicate))
{
}

void PhysicalFilter::open(dreamdb::executor::ExecutionContext & context)
{
    PhysicalUnaryOperator::open(context);
}

bool PhysicalFilter::next(dreamdb::executor::ExecutionContext & context, dreamdb::storage::Row & row)
{
    while (child_->next(context, row)) {
        if (evaluate(context, row)) {
            return true;
        }
    }
    return false;
}

void PhysicalFilter::close(dreamdb::executor::ExecutionContext & context)
{
    PhysicalUnaryOperator::close(context);
}

bool PhysicalFilter::evaluate(dreamdb::executor::ExecutionContext & context, const dreamdb::storage::Row & row) const
{
    // TODO: 评估过滤条件
    (void)context;
    (void)row;
    return false;
}

} // namespace dreamdb::planner::physical
