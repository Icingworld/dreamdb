#pragma once

#include <vector>

#include "dreamdb/planner/logical/logical_operator.h"
#include "dreamdb/common/ids.h"

namespace dreamdb::planner::logical
{

/**
 * @brief 逻辑扫描算子
 * @details 用于扫描数据集的算子
 */
class LogicalScan final : public LogicalOperator
{
public:
    LogicalScan(common::collection_id_t collection_id, std::vector<common::column_id_t> column_ids);

    ~LogicalScan() noexcept override = default;

public:
    /**
     * @brief 获取集合 ID
     * @return 集合 ID
     */
    common::collection_id_t collection_id() const noexcept;

    /**
     * @brief 获取列数量
     * @return 列数量
     */
    std::size_t column_count() const noexcept;

    /**
     * @brief 获取列 ID
     * @param index 列索引
     * @return 列 ID
     */
    common::column_id_t column_at(std::size_t index) const noexcept;

private:
    common::collection_id_t collection_id_;              // 集合 ID
    std::vector<common::column_id_t> column_ids_;        // 字段 ID 列表
};

} // namespace dreamdb::planner::logical
