#pragma once

#include <memory>

#include "dreamdb/common/mutation_result.h"
#include "dreamdb/planner/physical_planner/physical_plan_node.h"
#include "dreamdb/binder/bound/bound_statement.h"

namespace dreamdb
{

class DatabaseManager;

/**
 * @brief 执行器
 * @details Executor 负责执行 SQL 语句
 * - DML/DQL 语句经过 Planner 优化后，以 PhysicalPlanNode 形式执行
 * - DDL 语句直接以 BoundStatement 形式执行
 */
class Executor
{
public:
    /**
     * @brief 构造函数
     * @param database_manager 数据库管理器（所有权将被转移）
     */
    explicit Executor(std::unique_ptr<DatabaseManager> database_manager);

    Executor(const Executor &) = delete;

    Executor(Executor &&) noexcept = default;

    Executor & operator=(const Executor &) = delete;

    Executor & operator=(Executor &&) noexcept = default;

    ~Executor() = default;

public:
    /**
     * @brief 执行物理计划（用于 DML/DQL 语句，经过 planner 优化后）
     * @param physical_plan 物理计划节点
     * @return 执行结果
     */
    MutationResult execute(const PhysicalPlanNode & physical_plan);

    /**
     * @brief 执行绑定语句（用于 DDL 语句，直接执行）
     * @param bound_statement 绑定后的语句
     * @return 执行结果
     */
    MutationResult execute(const BoundStatement & bound_statement);

private:
    /**
     * @brief 执行 SELECT 物理计划
     * @param physical_plan 物理计划节点
     * @return 执行结果
     */
    MutationResult execute_select(const PhysicalPlanNode & physical_plan);

    /**
     * @brief 执行 UPDATE 物理计划
     * @param physical_plan 物理计划节点
     * @return 执行结果
     */
    MutationResult execute_update(const PhysicalPlanNode & physical_plan);

    /**
     * @brief 执行 DELETE 物理计划
     * @param physical_plan 物理计划节点
     * @return 执行结果
     */
    MutationResult execute_delete(const PhysicalPlanNode & physical_plan);

private:
    std::unique_ptr<DatabaseManager> database_manager_;  // 数据库管理器
};

} // namespace dreamdb
