#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include "dreamdb/planner/logical_planner/logical_plan_node.h"

namespace dreamdb
{

/**
 * @brief SELECT 逻辑计划节点类型
 */
enum class LogicalSelectPlanNodeType : std::uint8_t
{
    SELECT_SCAN,                   // 扫描集合
    SELECT_FILTER,                 // 过滤（WHERE 子句）
    SELECT_PROJECT,                // 投影（SELECT 列表）
    SELECT_AGGREGATE,              // 聚合（聚合函数，如 COUNT、SUM、AVG 等）
    SELECT_SORT,                   // 排序（ORDER BY 子句）
    SELECT_LIMIT,                  // 限制（LIMIT 子句）
    SELECT_OFFSET                  // 偏移（OFFSET 子句）
};

/**
 * @brief SELECT 逻辑计划节点基类
 * @details SELECT 语句专用的逻辑计划节点基类
 */
class LogicalSelectPlanNode : public LogicalPlanNode
{
public:
    explicit LogicalSelectPlanNode(LogicalSelectPlanNodeType type) noexcept;

    virtual ~LogicalSelectPlanNode() noexcept = default;

public:
    /**
     * @brief 获取节点类型
     * @return 节点类型
     */
    LogicalSelectPlanNodeType get_select_type() const noexcept;

protected:
    LogicalSelectPlanNodeType select_type_;   // SELECT 节点类型
};

} // namespace dreamdb
