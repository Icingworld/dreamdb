#include "dreamdb/planner/logical/operator/logical_scan.h"

#include <cassert>

namespace dreamdb::planner::logical
{

LogicalScan::LogicalScan(common::collection_id_t collection_id, std::vector<common::column_id_t> column_ids)
    : LogicalOperator(LogicalOperatorType::Scan)
    , collection_id_(collection_id)
    , column_ids_(std::move(column_ids))
{
    // 字段 ID 不能为空
    assert(!column_ids.empty());

    // Scan 算子不能持有子算子
    assert(child_count() == 0);
}

common::collection_id_t LogicalScan::collection_id() const noexcept
{
    return collection_id_;
}

std::size_t LogicalScan::column_count() const noexcept
{
    return column_ids_.size();
}

common::column_id_t LogicalScan::column_at(std::size_t index) const noexcept
{
    // 字段索引不能超过字段数量
    assert(index < column_ids_.size());

    return column_ids_.at(index);
}

} // namespace dreamdb::planner::logical
