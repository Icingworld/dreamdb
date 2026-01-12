#pragma once

#include <memory>

#include "dreamdb/planner/logical_planner/logical_plan_node.h"
#include "dreamdb/binder/bound/bound_statement.h"
#include "dreamdb/binder/bound/bound_select_statement.h"
#include "dreamdb/binder/bound/bound_insert_statement.h"
#include "dreamdb/binder/bound/bound_delete_statement.h"
#include "dreamdb/binder/bound/bound_update_statement.h"

namespace dreamdb
{

/**
 * @brief 逻辑计划器
 * @details LogicalPlanner 能够将绑定后的语句转换为 LogicalPlan
 * LogicalPlanner 的职责是对需要访问数据的查询语句进行计划，如 SELECT、INSERT、DELETE、UPDATE 等语句
 * 而数据库定义、表结构等元数据相关的操作则不经过 LogicalPlanner
 */
class LogicalPlanner
{
public:
    explicit LogicalPlanner() noexcept = default;

    LogicalPlanner(const LogicalPlanner &) noexcept = delete;

    LogicalPlanner(LogicalPlanner &&) noexcept = delete;

    LogicalPlanner & operator=(const LogicalPlanner &) noexcept = delete;

    LogicalPlanner & operator=(LogicalPlanner &&) noexcept = delete;

    ~LogicalPlanner() noexcept = default;

public:
    /**
     * @brief 计划节点
     * @param bound_statement 绑定后的语句
     * @return 逻辑计划节点
     */
    std::unique_ptr<LogicalPlanNode> plan(const BoundStatement & bound_statement) const;

private:
    /**
     * @brief 计划 SELECT 语句
     * @param bound_select_statement 绑定后的 SELECT 语句
     * @return 逻辑计划节点
     */
    std::unique_ptr<LogicalPlanNode> plan_select(const BoundSelectStatement & bound_select_statement) const;

    /**
     * @brief 计划 INSERT 语句
     * @param bound_insert_statement 绑定后的 INSERT 语句
     * @return 逻辑计划节点
     */
    std::unique_ptr<LogicalPlanNode> plan_insert(const BoundInsertStatement & bound_insert_statement) const;

    /**
     * @brief 计划 DELETE 语句
     * @param bound_delete_statement 绑定后的 DELETE 语句
     * @return 逻辑计划节点
     */
    std::unique_ptr<LogicalPlanNode> plan_delete(const BoundDeleteStatement & bound_delete_statement) const;

    /**
     * @brief 计划 UPDATE 语句
     * @param bound_update_statement 绑定后的 UPDATE 语句
     * @return 逻辑计划节点
     */
    std::unique_ptr<LogicalPlanNode> plan_update(const BoundUpdateStatement & bound_update_statement) const;
};

} // namespace dreamdb
