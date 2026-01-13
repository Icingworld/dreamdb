#pragma once

#include <memory>

#include "dreamdb/binder/bound/bound_statement.h"
#include "dreamdb/common/mutation_result.h"
#include "dreamdb/planner/logical_planner/logical_planner.h"
#include "dreamdb/planner/physical_planner/physical_planner.h"
#include "dreamdb/executor/executor.h"

namespace dreamdb
{

/**
 * @brief 调度器
 * @details Dispatcher 根据 BoundStatement 的类型决定执行路径：
 * - SELECT/UPDATE/DELETE: BoundStatement -> LogicalPlanner -> PhysicalPlanner -> Executor
 * - 其他语句（INSERT, CREATE, DROP, ALTER, DESCRIBE, SHOW, USE）: BoundStatement -> Executor
 */
class Dispatcher
{
public:
    /**
     * @brief 构造函数
     * @param executor 执行器引用
     * @param catalog Catalog 引用（用于 PhysicalPlanner）
     */
    explicit Dispatcher(Executor & executor, const Catalog & catalog) noexcept;

    Dispatcher(const Dispatcher &) noexcept = delete;

    Dispatcher(Dispatcher &&) noexcept = delete;

    Dispatcher & operator=(const Dispatcher &) noexcept = delete;

    Dispatcher & operator=(Dispatcher &&) noexcept = delete;

    ~Dispatcher() noexcept = default;

public:
    /**
     * @brief 调度并执行绑定后的语句
     * @param bound_statement 绑定后的语句
     * @return 执行结果
     */
    MutationResult dispatch(const BoundStatement & bound_statement);

private:
    /**
     * @brief 判断语句是否需要经过 Planner 路径
     * @param statement_type 语句类型
     * @return 如果需要经过 Planner 返回 true，否则返回 false
     */
    static bool needs_planner(BoundStatementType statement_type);

    Executor & executor_;                    // 执行器引用
    LogicalPlanner logical_planner_;          // 逻辑计划器
    PhysicalPlanner physical_planner_;       // 物理计划器
};

} // namespace dreamdb
