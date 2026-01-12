#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include "dreamdb/planner/physical_planner/physical_plan_node.h"

namespace dreamdb
{

/**
 * @brief SELECT 物理操作符类型
 */
enum class PhysicalSelectOperatorType : std::uint8_t
{
    SELECT_SEQ_SCAN,          // 全集合扫描
    SELECT_INDEX_SCAN,        // 索引扫描
    SELECT_VINDEX_SCAN,       // 向量索引扫描
    SELECT_FILTER,            // 过滤操作
    SELECT_PROJECT,           // 投影操作
    SELECT_AGGREGATE,         // 聚合操作
    SELECT_SORT,              // 排序操作
    SELECT_LIMIT_OFFSET       // 限制和偏移操作
};

/**
 * @brief SELECT 物理计划节点基类
 * @details SELECT 语句专用的物理计划节点基类
 */
class PhysicalSelectPlanNode : public PhysicalPlanNode
{
public:
    explicit PhysicalSelectPlanNode(PhysicalSelectOperatorType operator_type) noexcept;

    virtual ~PhysicalSelectPlanNode() noexcept = default;

public:
    /**
     * @brief 获取操作符类型
     * @return 操作符类型
     */
    PhysicalSelectOperatorType get_operator_type() const noexcept;

protected:
    PhysicalSelectOperatorType operator_type_;   // SELECT 操作符类型
};

} // namespace dreamdb
