#pragma once

#include <cstddef>
#include <optional>

#include "dreamdb/planner/physical_planner/select/physical_select_plan_node.h"

namespace dreamdb
{

/**
 * @brief 限制和偏移物理操作符
 * @details 合并了 LIMIT 和 OFFSET 功能，两者都是可选的
 */
class PhysicalLimitOffsetNode : public PhysicalSelectPlanNode
{
public:
    explicit PhysicalLimitOffsetNode(
        std::optional<std::size_t> limit,
        std::optional<std::size_t> offset
    ) noexcept;

    PhysicalLimitOffsetNode(const PhysicalLimitOffsetNode &) = delete;
    PhysicalLimitOffsetNode(PhysicalLimitOffsetNode &&) noexcept = default;
    PhysicalLimitOffsetNode & operator=(const PhysicalLimitOffsetNode &) = delete;
    PhysicalLimitOffsetNode & operator=(PhysicalLimitOffsetNode &&) noexcept = default;

    ~PhysicalLimitOffsetNode() override = default;

public:
    /**
     * @brief 获取限制数量
     * @return 限制数量，如果未设置返回 std::nullopt
     */
    std::optional<std::size_t> get_limit() const noexcept;

    /**
     * @brief 获取偏移量
     * @return 偏移量，如果未设置返回 std::nullopt
     */
    std::optional<std::size_t> get_offset() const noexcept;

private:
    std::optional<std::size_t> limit_;   // 限制数量
    std::optional<std::size_t> offset_; // 偏移量
};

} // namespace dreamdb
