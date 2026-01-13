#pragma once

#include <memory>

#include "dreamdb/planner/physical_planner/physical_plan_node.h"
#include "dreamdb/planner/logical_planner/logical_plan_node.h"
#include "dreamdb/planner/logical_planner/select/logical_select_plan_node.h"
#include "dreamdb/planner/logical_planner/update/logical_update_plan_node.h"
#include "dreamdb/planner/logical_planner/delete/logical_delete_plan_node.h"
#include "dreamdb/catalog/catalog.h"

namespace dreamdb
{

/**
 * @brief 物理计划器
 * @details PhysicalPlanner 能够将逻辑计划转换为物理执行计划
 * PhysicalPlanner 的职责是根据逻辑计划选择最优的物理执行策略，
 * 如选择扫描方式（全表扫描 vs 索引扫描）、连接算法、聚合算法等
 */
class PhysicalPlanner
{
public:
    explicit PhysicalPlanner(const Catalog & catalog) noexcept;

    PhysicalPlanner(const PhysicalPlanner &) noexcept = delete;

    PhysicalPlanner(PhysicalPlanner &&) noexcept = delete;

    PhysicalPlanner & operator=(const PhysicalPlanner &) noexcept = delete;

    PhysicalPlanner & operator=(PhysicalPlanner &&) noexcept = delete;

    ~PhysicalPlanner() noexcept = default;

public:
    /**
     * @brief 计划节点
     * @param logical_plan 逻辑计划节点
     * @return 物理计划节点
     */
    std::unique_ptr<PhysicalPlanNode> plan(const LogicalPlanNode & logical_plan) const;

private:
    /**
     * @brief 计划 SELECT 逻辑节点
     * @param logical_node SELECT 逻辑计划节点
     * @return 物理计划节点
     */
    std::unique_ptr<PhysicalPlanNode> plan_select(const LogicalSelectPlanNode & logical_node) const;

    /**
     * @brief 计划 UPDATE 逻辑节点
     * @param logical_node UPDATE 逻辑计划节点
     * @return 物理计划节点
     */
    std::unique_ptr<PhysicalPlanNode> plan_update(const LogicalUpdatePlanNode & logical_node) const;

    /**
     * @brief 计划 DELETE 逻辑节点
     * @param logical_node DELETE 逻辑计划节点
     * @return 物理计划节点
     */
    std::unique_ptr<PhysicalPlanNode> plan_delete(const LogicalDeletePlanNode & logical_node) const;

private:
    const Catalog & catalog_;  // Catalog 引用，用于获取元数据和索引信息
};

} // namespace dreamdb
