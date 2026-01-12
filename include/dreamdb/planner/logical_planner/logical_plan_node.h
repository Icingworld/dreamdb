#pragma once

#include <cstdint>
#include <vector>
#include <memory>

namespace dreamdb
{

/**
 * @brief 逻辑计划节点操作类型
 */
enum class LogicalPlanNodeOperationType : std::uint8_t
{
    LOGICAL_PLAN_SELECT,     // SELECT 操作
    LOGICAL_PLAN_INSERT,     // INSERT 操作
    LOGICAL_PLAN_UPDATE,     // UPDATE 操作
    LOGICAL_PLAN_DELETE      // DELETE 操作
};

/**
 * @brief 逻辑计划节点基类
 * @details 所有逻辑计划节点的基类，提供通用的树形结构支持
 */
class LogicalPlanNode
{
public:
    explicit LogicalPlanNode(LogicalPlanNodeOperationType operation_type) noexcept;

    virtual ~LogicalPlanNode() noexcept = default;

public:
    /**
     * @brief 获取操作类型
     * @return 操作类型
     */
    LogicalPlanNodeOperationType get_operation_type() const noexcept;

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
    LogicalPlanNodeOperationType operation_type_;              // 操作类型
    std::vector<std::unique_ptr<LogicalPlanNode>> children_;   // 子节点
};

} // namespace dreamdb
