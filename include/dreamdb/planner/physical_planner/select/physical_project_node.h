#pragma once

#include <string>
#include <vector>
#include <optional>
#include <memory>

#include "dreamdb/planner/physical_planner/select/physical_select_plan_node.h"
#include "dreamdb/expression/expression.h"

namespace dreamdb
{

/**
 * @brief 投影项
 */
struct PhysicalProjectItem
{
    std::unique_ptr<Expression> expression;    // 投影表达式
    std::optional<std::string> alias;          // 可选别名
};

/**
 * @brief 投影物理操作符
 */
class PhysicalProjectNode : public PhysicalSelectPlanNode
{
public:
    explicit PhysicalProjectNode(std::vector<PhysicalProjectItem> project_items);

    PhysicalProjectNode(const PhysicalProjectNode &) = delete;
    PhysicalProjectNode(PhysicalProjectNode &&) noexcept = default;
    PhysicalProjectNode & operator=(const PhysicalProjectNode &) = delete;
    PhysicalProjectNode & operator=(PhysicalProjectNode &&) noexcept = default;

    ~PhysicalProjectNode() override = default;

public:
    /**
     * @brief 获取投影项列表
     * @return 投影项列表
     */
    const std::vector<PhysicalProjectItem> & get_project_items() const noexcept;

    /**
     * @brief 获取可变投影项列表
     * @return 投影项列表
     */
    std::vector<PhysicalProjectItem> & get_mutable_project_items() noexcept;

private:
    std::vector<PhysicalProjectItem> project_items_;    // 投影项列表
};

} // namespace dreamdb
