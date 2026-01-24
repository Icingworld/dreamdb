#pragma once

#include <memory>

namespace dreamdb::binder::bound
{

class BoundStatement;
class BoundSelectStatement;
class BoundDeleteStatement;
class BoundUpdateStatement;

} // namespace dreamdb::binder::bound

namespace dreamdb::planner::logical
{

class LogicalOperator;

/**
 * @brief 逻辑计划器
 */
class LogicalPlanner
{
public:
    explicit LogicalPlanner() noexcept = default;

    ~LogicalPlanner() noexcept = default;

public:
    /**
     * @brief 计划节点
     * @param bound_statement 绑定后的语句
     * @return 逻辑计划节点
     */
    std::unique_ptr<LogicalOperator> plan(
        const dreamdb::binder::bound::BoundStatement & bound_statement
    ) const;

private:
    /**
     * @brief 计划 SELECT 语句
     * @param bound_select_statement 绑定后的 SELECT 语句
     * @return 逻辑计划节点
     */
    std::unique_ptr<LogicalOperator> plan_select(
        const dreamdb::binder::bound::BoundSelectStatement & select_statement
    ) const;

    /**
     * @brief 计划 DELETE 语句
     * @param bound_delete_statement 绑定后的 DELETE 语句
     * @return 逻辑计划节点
     */
    std::unique_ptr<LogicalOperator> plan_delete(
        const dreamdb::binder::bound::BoundDeleteStatement & delete_statement
    ) const;

    /**
     * @brief 计划 UPDATE 语句
     * @param bound_update_statement 绑定后的 UPDATE 语句
     * @return 逻辑计划节点
     */
    std::unique_ptr<LogicalOperator> plan_update(
        const dreamdb::binder::bound::BoundUpdateStatement & update_statement
    ) const;
};

} // namespace dreamdb::planner::logical
