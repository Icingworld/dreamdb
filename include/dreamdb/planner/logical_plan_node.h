#pragma once

#include <cstdint>
#include <vector>
#include <memory>

namespace dreamdb
{

/**
 * @brief 逻辑计划节点类型
 */
enum class LogicalPlanNodeType : std::uint8_t
{
    SCAN,                   // 扫描集合
    FILTER,                 // 过滤（WHERE 子句）
    PROJECT,                // 投影（SELECT 列表）
    AGGREGATE,              // 聚合（聚合函数，如 COUNT、SUM、AVG 等）
    SORT,                   // 排序（ORDER BY 子句）
    LIMIT,                  // 限制（LIMIT 子句）
    OFFSET                  // 偏移（OFFSET 子句）
};

/**
 * @brief 逻辑计划节点
 */
class LogicalPlanNode
{
public:
    explicit LogicalPlanNode(LogicalPlanNodeType type);

    virtual ~LogicalPlanNode() = default;

public:
    /**
     * @brief 获取节点类型
     * @return 节点类型
     */
    LogicalPlanNodeType get_type() const noexcept;

    /**
     * @brief 获取子节点列表
     * @return 子节点列表
     */
    const std::vector<std::unique_ptr<LogicalPlanNode>> & get_children() const noexcept;

    /**
     * @brief 获取可变子节点列表
     * @return 子节点列表
     */
    std::vector<std::unique_ptr<LogicalPlanNode>> & get_mutable_children() noexcept;

protected:
    LogicalPlanNodeType type_;   // 节点类型
    std::vector<std::unique_ptr<LogicalPlanNode>> children_;   // 子节点
};

} // namespace dreamdb
