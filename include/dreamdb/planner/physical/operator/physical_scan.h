#pragma once

#include <vector>

#include "dreamdb/planner/physical/physical_operator.h"
#include "dreamdb/common/ids.h"

namespace dreamdb::planner::physical
{

/**
 * @brief 物理扫描算子
 */
class PhysicalScan final : public PhysicalOperator
{
public:
    explicit PhysicalScan(
        dreamdb::common::collection_id_t collection_id, std::vector<dreamdb::common::column_id_t> column_ids
    );

    ~PhysicalScan() override = default;

public:
    /**
     * @brief 获取集合 ID
     * @return 集合 ID
     */
    dreamdb::common::collection_id_t collection_id() const;

    /**
     * @brief 获取列 ID 数量
     * @return 列 ID 数量
     */
    std::size_t column_id_count() const;

    /**
     * @brief 获取列 ID 
     * @param index 列索引
     * @return 列 ID
     */
    dreamdb::common::column_id_t column_id_at(std::size_t index) const;

    /**
     * @brief 打开算子
     * @param context 执行上下文
     */
    void open(ExecutionContext & context) override;

    /**
     * @brief 获取下一行
     * @param context 执行上下文
     * @param rowOut 行输出
     * @return 是否还有下一行
     */
    bool next(ExecutionContext & context, RowOut & rowOut) override;

    /**
     * @brief 关闭算子
     * @param context 执行上下文
     */
    void close(ExecutionContext & context) override;

private:
    dreamdb::common::collection_id_t collection_id_;          // 集合 ID
    std::vector<dreamdb::common::column_id_t> column_ids_;    // 列 ID 列表
};

} // namespace dreamdb::planner::physical
