#include "dreamdb/executor/pipeline_executor.h"

#include "dreamdb/planner/physical/physical_plan.h"
#include "dreamdb/planner/physical/physical_operator.h"

namespace dreamdb::executor
{

PipelineExecutor::PipelineExecutor()
{
}

ExecutionResult PipelineExecutor::execute(planner::physical::PhysicalPlan & physical_plan, ExecutionContext & context)
{
    if (physical_plan.result_type() == dreamdb::planner::physical::ResultType::Query) {
        return execute_query(physical_plan, static_cast<ExecutionContextReadOnly &>(context));
    } else {
        return execute_mutation(physical_plan, static_cast<ExecutionContextWritable &>(context));
    }
}

ExecutionResult PipelineExecutor::execute_query(planner::physical::PhysicalPlan & physical_plan, ExecutionContextReadOnly & context)
{
    QueryResult query_result;

    // 打开上下文
    physical_plan.root().open(context);

    // 循环拉取行，直到没有行
    dreamdb::storage::Row row;
    while (physical_plan.root().next(context, row)) {
        // 对于 DQL 来说，需要处理输出内容
        query_result.rows.push_back(row);
        row.values.clear();
    }

    // 处理完毕，关闭上下文
    physical_plan.root().close(context);

    return query_result;
}

ExecutionResult PipelineExecutor::execute_mutation(planner::physical::PhysicalPlan & physical_plan, ExecutionContextWritable & context)
{
    MutationResult mutation_result;

    // 打开上下文
    physical_plan.root().open(context);

    // DML 其实不需要修改数据，这里构造一个不使用的行
    dreamdb::storage::Row dummy_row;

    // 循环拉取行，直到没有行
    while (physical_plan.root().next(context, dummy_row)) {
        // 对于 DML 来说，不需要处理输出内容
        dummy_row.values.clear();
    }

    // 处理完毕，关闭上下文
    physical_plan.root().close(context);

    // 从算子中取出受影响的行数
    // TODO：让算子继承一个携带计数器的基类
    // std::uint64_t affected_rows = physical_plan.root().affected_rows();

    return mutation_result;
}

} // namespace dreamdb::executor
