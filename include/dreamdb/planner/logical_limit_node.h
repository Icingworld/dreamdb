#pragma once

#include "dreamdb/planner/logical_plan_node.h"

namespace dreamdb
{

/**
 * @brief 限制节点
 */
class LogicalLimitNode : public LogicalPlanNode
{
public:
    explicit LogicalLimitNode(std::size_t limit) noexcept;

    LogicalLimitNode(const LogicalLimitNode &) = delete;

    LogicalLimitNode(LogicalLimitNode &&) noexcept = default;

    LogicalLimitNode & operator=(const LogicalLimitNode &) = delete;

    LogicalLimitNode & operator=(LogicalLimitNode &&) noexcept = default;

    ~LogicalLimitNode() override = default;

public:
    /**
     * @brief 获取限制数量
     * @return 限制数量
     */
    std::size_t get_limit() const noexcept;

private:
    std::size_t limit_;   // 限制数量
};

} // namespace dreamdb
