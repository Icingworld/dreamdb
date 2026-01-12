#pragma once

#include <cstdint>
#include <vector>
#include <memory>

namespace dreamdb
{

/**
 * @brief 物理计划节点操作类型
 */
enum class PhysicalPlanNodeOperationType : std::uint8_t
{
    PHYSICAL_PLAN_SELECT,     // SELECT 操作
    PHYSICAL_PLAN_UPDATE,     // UPDATE 操作
    PHYSICAL_PLAN_DELETE      // DELETE 操作
};

/**
 * @brief 物理计划节点基类
 * @details 所有物理计划节点的基类，提供通用的树形结构支持
 */
class PhysicalPlanNode
{
public:
    explicit PhysicalPlanNode(PhysicalPlanNodeOperationType operation_type) noexcept;

    virtual ~PhysicalPlanNode() noexcept = default;

public:
    /**
     * @brief 获取操作类型
     * @return 操作类型
     */
    PhysicalPlanNodeOperationType get_operation_type() const noexcept;

    /**
     * @brief 获取子节点列表
     * @return 子节点列表
     */
    const std::vector<std::unique_ptr<PhysicalPlanNode>> & get_children() const noexcept;

    /**
     * @brief 获取可变子节点列表
     * @return 子节点列表
     */
    std::vector<std::unique_ptr<PhysicalPlanNode>> & get_mutable_children() noexcept;

protected:
    PhysicalPlanNodeOperationType operation_type_;              // 操作类型
    std::vector<std::unique_ptr<PhysicalPlanNode>> children_;   // 子节点
};

} // namespace dreamdb
