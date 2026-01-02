#pragma once

#include <memory>

#include "dreamdb/planner/logical_plan_node.h"
#include "dreamdb/parser/ast/ast_node.h"
#include "dreamdb/parser/ast/select_stmt.h"
#include "dreamdb/parser/ast/delete_stmt.h"
#include "dreamdb/parser/ast/update_stmt.h"

namespace dreamdb
{

/**
 * @brief 逻辑计划器
 * @details LogicalPlanner 能够将解析器生成的抽象语法树转换为 LogicalPlan
 * LogicalPlanner 的职责是对需要访问数据的查询语句进行计划，如 SELECT、DELETE、UPDATE 等语句
 * 而数据库定义、表结构等元数据相关的操作则不经过 LogicalPlanner
 */
class LogicalPlanner
{
public:
    LogicalPlanner();

    LogicalPlanner(const LogicalPlanner &) noexcept = delete;

    LogicalPlanner(LogicalPlanner &&) noexcept = delete;

    LogicalPlanner & operator=(const LogicalPlanner &) noexcept = delete;

    LogicalPlanner & operator=(LogicalPlanner &&) noexcept = delete;

    ~LogicalPlanner() noexcept = default;

public:
    /**
     * @brief 计划节点
     * @param ast 抽象语法树
     * @return 逻辑计划节点
     * @details 关于抽象语法树，Planner 不打算持有其所有权，而是只读取其内容，它的所有权仍然属于 Parser
     */
    std::unique_ptr<LogicalPlanNode> plan(const AstNode & ast) const;

private:
    /**
     * @brief 计划 SELECT 语句
     * @param select_stmt SELECT 语法树
     * @return 逻辑计划节点
     */
    std::unique_ptr<LogicalPlanNode> plan_select(const SelectStmt & select_stmt) const;

    /**
     * @brief 计划 DELETE 语句
     * @param delete_stmt DELETE 语法树
     * @return 逻辑计划节点
     */
    std::unique_ptr<LogicalPlanNode> plan_delete(const DeleteStmt & delete_stmt) const;

    /**
     * @brief 计划 UPDATE 语句
     * @param update_stmt UPDATE 语法树
     * @return 逻辑计划节点
     */
    std::unique_ptr<LogicalPlanNode> plan_update(const UpdateStmt & update_stmt) const;
};

} // namespace dreamdb
