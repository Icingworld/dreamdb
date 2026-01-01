#pragma once

#include <string>
#include <vector>
#include <optional>
#include <memory>

#include "dreamdb/planner/logical_plan_node.h"
#include "dreamdb/expression/expression.h"

namespace dreamdb
{

/**
 * @brief 投影项
 */
struct ProjectItem
{
    std::unique_ptr<Expression> expression;    // 投影表达式
    std::optional<std::string> alias;          // 可选别名
};

/**
 * @brief 投影节点
 */
class LogicalProjectNode : public LogicalPlanNode
{
public:
    explicit LogicalProjectNode(std::vector<ProjectItem> project_items);

    LogicalProjectNode(const LogicalProjectNode &) noexcept = delete;

    LogicalProjectNode(LogicalProjectNode &&) noexcept = default;

    LogicalProjectNode & operator=(const LogicalProjectNode &) noexcept = delete;

    LogicalProjectNode & operator=(LogicalProjectNode &&) noexcept = default;

public:
    /**
     * @brief 获取投影项列表
     * @return 投影项列表
     */
    const std::vector<ProjectItem> & get_project_items() const noexcept;

    /**
     * @brief 获取可变投影项列表
     * @return 投影项列表
     */
    std::vector<ProjectItem> & get_mutable_project_items() noexcept;

private:
    std::vector<ProjectItem> project_items_;    // 投影项列表
};

} // namespace dreamdb
