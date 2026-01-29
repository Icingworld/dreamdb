#pragma once

#include "dreamdb/executor/execution_result.h"
#include "dreamdb/executor/context.h"

namespace dreamdb::planner::physical
{

class PhysicalPlan;

} // namespace dreamdb::planner::physical

namespace dreamdb::executor
{

/**
 * @brief 管线执行器
 * @details PipelineExecutor 负责执行被 Planner 优化后的物理算子
 */
class PipelineExecutor
{
public:
    explicit PipelineExecutor();

    ~PipelineExecutor() = default;

public:
    /**
     * @brief 执行物理计划
     * @param physical_plan 物理计划
     * @return 执行结果
     */
    ExecutionResult execute(planner::physical::PhysicalPlan & physical_plan, ExecutionContext & context);

private:
    /**
     * @brief 执行查询计划
     * @param physical_plan 物理计划
     * @param context 执行上下文
     * @return 执行结果
     */
    ExecutionResult execute_query(planner::physical::PhysicalPlan & physical_plan, ExecutionContextReadOnly & context);

    /**
     * @brief 执行修改计划
     * @param physical_plan 物理计划
     * @param context 执行上下文
     * @return 执行结果
     */
    ExecutionResult execute_mutation(planner::physical::PhysicalPlan & physical_plan, ExecutionContextWritable & context);
};

} // namespace dreamdb::executor
