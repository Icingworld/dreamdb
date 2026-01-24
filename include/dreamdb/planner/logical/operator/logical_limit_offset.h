#pragma once

#include <optional>

#include "dreamdb/planner/logical/logical_operator.h"

namespace dreamdb::planner::logical
{

/**
 * @brief 逻辑限制偏移算子
 * @details 用于限制和偏移结果的算子
 */
class LogicalLimitOffset final : public LogicalOperator
{
public:
    LogicalLimitOffset(std::optional<std::size_t> limit, std::optional<std::size_t> offset);

    ~LogicalLimitOffset() noexcept override = default;

public:
    /**
     * @brief 获取限制
     * @return 限制
     */
    std::optional<std::size_t> limit() const noexcept;

    /**
     * @brief 获取偏移
     * @return 偏移
     */
    std::optional<std::size_t> offset() const noexcept;

private:
    std::optional<std::size_t> limit_;       // 限制
    std::optional<std::size_t> offset_;      // 偏移
};

} // namespace dreamdb::planner::logical
