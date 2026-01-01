#include "dreamdb/planner/planner.h"

#include <stdexcept>

namespace dreamdb
{

std::unique_ptr<LogicalPlanNode> Planner::plan(const AstNode & ast) const
{
    // 根据抽象语法树的类型，选择对应的计划
    switch (ast.get_type()) {
        case AstNodeType::SELECT_STMT:
            return plan_select(static_cast<const SelectStmt &>(ast));
        case AstNodeType::DELETE_STMT:
            return plan_delete(static_cast<const DeleteStmt &>(ast));
        case AstNodeType::UPDATE_STMT:
            return plan_update(static_cast<const UpdateStmt &>(ast));
        default:
            throw std::runtime_error("Unsupported AST node type: " + std::to_string(static_cast<int>(ast.get_type())));
    }
}

std::unique_ptr<LogicalPlanNode> Planner::plan_select(const SelectStmt & select_stmt) const
{
    (void)select_stmt;
    throw std::runtime_error("Select plan is not implemented");
}

std::unique_ptr<LogicalPlanNode> Planner::plan_delete(const DeleteStmt & delete_stmt) const
{
    (void)delete_stmt;
    throw std::runtime_error("Delete plan is not implemented");
}

std::unique_ptr<LogicalPlanNode> Planner::plan_update(const UpdateStmt & update_stmt) const
{
    (void)update_stmt;
    throw std::runtime_error("Update plan is not implemented");
}

} // namespace dreamdb
