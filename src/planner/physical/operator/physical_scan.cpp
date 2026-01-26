#include "dreamdb/planner/physical/operator/physical_scan.h"

namespace dreamdb::planner::physical
{

PhysicalScan::PhysicalScan(
    dreamdb::common::collection_id_t collection_id, std::vector<dreamdb::common::column_id_t> column_ids
)
    : collection_id_(collection_id)
    , column_ids_(std::move(column_ids))
{
}

dreamdb::common::collection_id_t PhysicalScan::collection_id() const
{
    return collection_id_;
}

std::size_t PhysicalScan::column_id_count() const
{
    return column_ids_.size();
}

dreamdb::common::column_id_t PhysicalScan::column_id_at(std::size_t index) const
{
    return column_ids_.at(index);
}

void PhysicalScan::open(ExecutionContext & context)
{
    // TODO: 实现打开算子
    (void)context;
}

bool PhysicalScan::next(ExecutionContext & context, RowOut & rowOut)
{
    // TODO: 实现获取下一行
    (void)context;
    (void)rowOut;
    return false;
}

void PhysicalScan::close(ExecutionContext & context)
{
    // TODO: 实现关闭算子
    (void)context;
}

} // namespace dreamdb::planner::physical
