#include "dreamdb/dispatcher/dispatcher.h"

#include "dreamdb/binder/bound/bound_select_statement.h"
#include "dreamdb/binder/bound/bound_update_statement.h"
#include "dreamdb/binder/bound/bound_delete_statement.h"
#include "dreamdb/planner/logical_planner/logical_plan_node.h"

namespace dreamdb
{

Dispatcher::Dispatcher(Executor & executor, const Catalog & catalog) noexcept
    : executor_(executor)
    , logical_planner_()
    , physical_planner_(catalog)
{
}

MutationResult Dispatcher::dispatch(const BoundStatement & bound_statement)
{
    // 判断是否需要经过 Planner 路径
    if (needs_planner(bound_statement.get_type())) {
        // 需要经过 Planner 的语句：SELECT, UPDATE, DELETE
        // 1. Logical Planner: BoundStatement -> LogicalPlanNode
        std::unique_ptr<LogicalPlanNode> logical_plan = logical_planner_.plan(bound_statement);
        
        if (!logical_plan) {
            return MutationResult::make_failure("Failed to create logical plan");
        }
        
        // 2. Physical Planner: LogicalPlanNode -> PhysicalPlanNode
        std::unique_ptr<PhysicalPlanNode> physical_plan = physical_planner_.plan(*logical_plan);
        
        if (!physical_plan) {
            return MutationResult::make_failure("Failed to create physical plan");
        }
        
        // 3. Executor: PhysicalPlanNode -> MutationResult
        return executor_.execute(*physical_plan);
    }
    else {
        // 直接执行的语句：INSERT, CREATE, DROP, ALTER, DESCRIBE, SHOW, USE
        return executor_.execute(bound_statement);
    }
}

bool Dispatcher::needs_planner(BoundStatementType statement_type)
{
    switch (statement_type) {
        case BoundStatementType::BINDER_BOUND_SELECT_STATEMENT:
        case BoundStatementType::BINDER_BOUND_UPDATE_STATEMENT:
        case BoundStatementType::BINDER_BOUND_DELETE_STATEMENT:
            return true;
        case BoundStatementType::BINDER_BOUND_INSERT_STATEMENT:
        case BoundStatementType::BINDER_BOUND_USE_STATEMENT:
        case BoundStatementType::BINDER_BOUND_SHOW_STATEMENT:
        case BoundStatementType::BINDER_BOUND_DESCRIBE_STATEMENT:
        case BoundStatementType::BINDER_BOUND_DROP_STATEMENT:
        case BoundStatementType::BINDER_BOUND_CREATE_STATEMENT:
        case BoundStatementType::BINDER_BOUND_ALTER_STATEMENT:
            return false;
        default:
            return false;
    }
}

} // namespace dreamdb
