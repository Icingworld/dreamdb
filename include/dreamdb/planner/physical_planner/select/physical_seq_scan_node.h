#pragma once

#include <cstddef>
#include <vector>

#include "dreamdb/planner/physical_planner/select/physical_select_plan_node.h"

namespace dreamdb
{

/**
 * @brief 全表扫描物理操作符
 */
class PhysicalSeqScanNode : public PhysicalSelectPlanNode
{
public:
    explicit PhysicalSeqScanNode(
        std::size_t collection_id,
        std::vector<std::size_t> field_indexes
    );

    PhysicalSeqScanNode(const PhysicalSeqScanNode &) = delete;
    PhysicalSeqScanNode(PhysicalSeqScanNode &&) noexcept = default;
    PhysicalSeqScanNode & operator=(const PhysicalSeqScanNode &) = delete;
    PhysicalSeqScanNode & operator=(PhysicalSeqScanNode &&) noexcept = default;

    ~PhysicalSeqScanNode() override = default;

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
