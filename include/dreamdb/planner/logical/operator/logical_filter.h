#pragma once

#include "dreamdb/planner/logical/logical_operator.h"

namespace dreamdb::binder::bound
{

class BoundExpression;

} // namespace dreamdb::binder::bound

namespace dreamdb::planner::logical
{

/**
 * @brief 逻辑过滤算子
 * @details 用于过滤的算子
 */
class LogicalFilter final : public LogicalOperator
{
public:
    LogicalFilter(std::unique_ptr<dreamdb::binder::bound::BoundExpression> predicate);

    ~LogicalFilter() noexcept override = default;

public:
    /**
     * @brief 获取谓词表达式
     * @return 谓词表达式
     */
    const dreamdb::binder::bound::BoundExpression & predicate() const noexcept;

private:
    std::unique_ptr<dreamdb::binder::bound::BoundExpression> predicate_;    // 谓词表达式
};

} // namespace dreamdb::planner::logical
