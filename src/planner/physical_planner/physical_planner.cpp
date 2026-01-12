#include "dreamdb/planner/physical_planner/physical_planner.h"

#include <stdexcept>

#include "dreamdb/planner/logical_planner/select/logical_select_plan_node.h"
#include "dreamdb/planner/logical_planner/insert/logical_insert_plan_node.h"
#include "dreamdb/planner/logical_planner/update/logical_update_plan_node.h"
#include "dreamdb/planner/logical_planner/delete/logical_delete_plan_node.h"

namespace dreamdb
{

PhysicalPlanner::PhysicalPlanner(const Catalog & catalog) noexcept
    : catalog_(catalog)
{
}

std::unique_ptr<PhysicalPlanNode> PhysicalPlanner::plan(const LogicalPlanNode & logical_plan) const
{
    // 根据逻辑计划的操作类型，选择对应的物理计划
    switch (logical_plan.get_operation_type()) {
        case LogicalPlanNodeOperationType::LOGICAL_PLAN_SELECT:
            return plan_select(logical_plan);
        case LogicalPlanNodeOperationType::LOGICAL_PLAN_INSERT:
            return plan_insert(logical_plan);
        case LogicalPlanNodeOperationType::LOGICAL_PLAN_UPDATE:
            return plan_update(logical_plan);
        case LogicalPlanNodeOperationType::LOGICAL_PLAN_DELETE:
            return plan_delete(logical_plan);
        default:
            throw std::runtime_error("Unsupported logical plan operation type: " + 
                std::to_string(static_cast<int>(logical_plan.get_operation_type())));
    }
}

std::unique_ptr<PhysicalPlanNode> PhysicalPlanner::plan_select(const LogicalPlanNode & logical_node) const
{
    (void)logical_node;
    throw std::runtime_error("Select physical plan is not implemented");
}

std::unique_ptr<PhysicalPlanNode> PhysicalPlanner::plan_insert(const LogicalPlanNode & logical_node) const
{
    (void)logical_node;
    throw std::runtime_error("Insert physical plan is not implemented");
}

std::unique_ptr<PhysicalPlanNode> PhysicalPlanner::plan_update(const LogicalPlanNode & logical_node) const
{
    (void)logical_node;
    throw std::runtime_error("Update physical plan is not implemented");
}

std::unique_ptr<PhysicalPlanNode> PhysicalPlanner::plan_delete(const LogicalPlanNode & logical_node) const
{
    (void)logical_node;
    throw std::runtime_error("Delete physical plan is not implemented");
}

} // namespace dreamdb
