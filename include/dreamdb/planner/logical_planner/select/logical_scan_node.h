#pragma once

#include <cstddef>
#include <vector>

#include "dreamdb/planner/logical_planner/select/logical_select_plan_node.h"

namespace dreamdb
{

class LogicalScanNode : public LogicalSelectPlanNode
{
public:
    explicit LogicalScanNode(std::size_t collection_id, std::vector<std::size_t> field_indexes);

    LogicalScanNode(const LogicalScanNode &) = delete;

    LogicalScanNode(LogicalScanNode &&) noexcept = default;

    LogicalScanNode & operator=(const LogicalScanNode &) = delete;

    LogicalScanNode & operator=(LogicalScanNode &&) noexcept = default;

    ~LogicalScanNode() override = default;

public:
    /**
     * @brief 获取集合 ID
     * @return 集合 ID
     */
    std::size_t get_collection_id() const noexcept;

    /**
     * @brief 获取字段索引列表
     * @return 字段索引列表
     */
    const std::vector<std::size_t> & get_field_indexes() const noexcept;

private:
    std::size_t collection_id_;                // 集合 ID
    std::vector<std::size_t> field_indexes_;   // 字段索引列表
};

} // namespace dreamdb
