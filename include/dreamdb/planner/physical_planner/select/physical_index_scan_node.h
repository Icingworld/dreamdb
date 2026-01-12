#pragma once

#include <cstddef>
#include <vector>
#include <string>

#include "dreamdb/planner/physical_planner/select/physical_select_plan_node.h"

namespace dreamdb
{

/**
 * @brief 索引扫描物理操作符
 */
class PhysicalIndexScanNode : public PhysicalSelectPlanNode
{
public:
    explicit PhysicalIndexScanNode(
        std::size_t collection_id,
        const std::string & index_name,
        std::vector<std::size_t> field_indexes
    );

    PhysicalIndexScanNode(const PhysicalIndexScanNode &) = delete;
    PhysicalIndexScanNode(PhysicalIndexScanNode &&) noexcept = default;
    PhysicalIndexScanNode & operator=(const PhysicalIndexScanNode &) = delete;
    PhysicalIndexScanNode & operator=(PhysicalIndexScanNode &&) noexcept = default;

    ~PhysicalIndexScanNode() override = default;

public:
    /**
     * @brief 获取集合 ID
     * @return 集合 ID
     */
    std::size_t get_collection_id() const noexcept;

    /**
     * @brief 获取索引名称
     * @return 索引名称
     */
    const std::string & get_index_name() const noexcept;

    /**
     * @brief 获取字段索引列表
     * @return 字段索引列表
     */
    const std::vector<std::size_t> & get_field_indexes() const noexcept;

private:
    std::size_t collection_id_;                // 集合 ID
    std::string index_name_;                   // 索引名称
    std::vector<std::size_t> field_indexes_;   // 字段索引列表
};

} // namespace dreamdb
