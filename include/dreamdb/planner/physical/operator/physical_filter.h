#pragma once

#include "dreamdb/planner/physical/operator/physical_unary_operator.h"

namespace dreamdb::binder::bound
{

class BoundExpression;

} // namespace dreamdb::binder::bound

namespace dreamdb::planner::physical
{

/**
 * @brief 物理过滤算子
 */
class PhysicalFilter final : public PhysicalUnaryOperator
{
public:
    explicit PhysicalFilter(
        std::unique_ptr<PhysicalOperator> child,
        std::unique_ptr<dreamdb::binder::bound::BoundExpression> predicate
    );

    ~PhysicalFilter() override = default;

public:
    
};

} // namespace dreamdb::planner::physical
