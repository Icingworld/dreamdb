#pragma once

#include "dreamdb/planner/logical_plan_node.h"

namespace dreamdb
{

/**
 * @brief 偏移节点
 */
class LogicalOffsetNode : public LogicalPlanNode
{
public:
    explicit LogicalOffsetNode(std::size_t offset) noexcept;

    LogicalOffsetNode(const LogicalOffsetNode &) = delete;

    LogicalOffsetNode(LogicalOffsetNode &&) noexcept = default;

    LogicalOffsetNode & operator=(const LogicalOffsetNode &) = delete;

    LogicalOffsetNode & operator=(LogicalOffsetNode &&) noexcept = default;

    ~LogicalOffsetNode() override = default;

public:
    /**
     * @brief 获取偏移量
     * @return 偏移量
     */
    std::size_t get_offset() const noexcept;

private:
    std::size_t offset_;   // 偏移量
};

} // namespace dreamdb
