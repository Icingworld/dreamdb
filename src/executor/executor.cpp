#include "dreamdb/executor/executor.h"

#include "dreamdb/schema/database_manager.h"

namespace dreamdb
{

Executor::Executor(std::unique_ptr<DatabaseManager> database_manager)
    : database_manager_(std::move(database_manager))
{
}

MutationResult Executor::execute(const PhysicalPlanNode & physical_plan)
{
    switch (physical_plan.get_operation_type())
    {
        case PhysicalPlanNodeOperationType::PHYSICAL_PLAN_SELECT:
            return execute_select(physical_plan);
        case PhysicalPlanNodeOperationType::PHYSICAL_PLAN_UPDATE:
            return execute_update(physical_plan);
        case PhysicalPlanNodeOperationType::PHYSICAL_PLAN_DELETE:
            return execute_delete(physical_plan);
        default:
            return MutationResult::make_failure("Unsupported physical plan operation type");
    }
}

MutationResult Executor::execute(const BoundStatement & /*bound_statement*/)
{
    // DDL 语句直接执行
    return MutationResult::make_failure("DDL execution not implemented yet");
}

MutationResult Executor::execute_select(const PhysicalPlanNode & /*physical_plan*/)
{
    // TODO: 实现 SELECT 执行逻辑
    return MutationResult::make_failure("SELECT execution not implemented yet");
}

MutationResult Executor::execute_update(const PhysicalPlanNode & /*physical_plan*/)
{
    // TODO: 实现 UPDATE 执行逻辑
    return MutationResult::make_failure("UPDATE execution not implemented yet");
}

MutationResult Executor::execute_delete(const PhysicalPlanNode & /*physical_plan*/)
{
    // TODO: 实现 DELETE 执行逻辑
    return MutationResult::make_failure("DELETE execution not implemented yet");
}

} // namespace dreamdb
