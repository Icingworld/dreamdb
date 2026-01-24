#include "dreamdb/planner/logical/logical_planner.h"

#include <stdexcept>

#include "dreamdb/binder/bound/statement/statement.h"
#include "dreamdb/binder/bound/statement/select.h"
#include "dreamdb/binder/bound/statement/delete.h"
#include "dreamdb/binder/bound/statement/update.h"

namespace dreamdb::planner::logical
{

std::unique_ptr<LogicalOperator> LogicalPlanner::plan(
    const dreamdb::binder::bound::BoundStatement & bound_statement
) const
{
    // 根据绑定后的语句的类型，选择对应的计划
    switch (bound_statement.statement_type()) {
        case dreamdb::binder::bound::BoundStatementType::Select:
            return plan_select(
                static_cast<const dreamdb::binder::bound::BoundSelectStatement &>(bound_statement)
            );
        case dreamdb::binder::bound::BoundStatementType::Delete:
            return plan_delete(
                static_cast<const dreamdb::binder::bound::BoundDeleteStatement &>(bound_statement)
            );
        case dreamdb::binder::bound::BoundStatementType::Update:
            return plan_update(
                static_cast<const dreamdb::binder::bound::BoundUpdateStatement &>(bound_statement)
            );
        default:
            throw std::runtime_error(
                "Unsupported bound statement type: " +
                std::to_string(static_cast<std::uint8_t>(bound_statement.statement_type()))
            );
    }
}

std::unique_ptr<LogicalOperator> LogicalPlanner::plan_select(
    const dreamdb::binder::bound::BoundSelectStatement & select_statement
) const
{
    (void)select_statement;
    return nullptr;
}

std::unique_ptr<LogicalOperator> LogicalPlanner::plan_delete(
    const dreamdb::binder::bound::BoundDeleteStatement & delete_statement
) const
{
    (void)delete_statement;
    return nullptr;
}

std::unique_ptr<LogicalOperator> LogicalPlanner::plan_update(
    const dreamdb::binder::bound::BoundUpdateStatement & update_statement
) const
{
    (void)update_statement;
    return nullptr;
}

} // namespace dreamdb::planner::logical
