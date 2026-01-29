#include "dreamdb/planner/physical/operator/physical_scan.h"

namespace dreamdb::planner::physical
{

PhysicalScan::PhysicalScan(
    dreamdb::common::collection_id_t collection_id, std::vector<dreamdb::common::column_oid_t> column_ids
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

dreamdb::common::column_oid_t PhysicalScan::column_id_at(std::size_t index) const
{
    return column_ids_.at(index);
}

void PhysicalScan::open(dreamdb::executor::ExecutionContext & context)
{
    // TODO: 实现打开算子
    (void)context;
}

bool PhysicalScan::next(dreamdb::executor::ExecutionContext & context, dreamdb::storage::Row & row)
{
    // TODO: 实现获取下一行
    (void)context;
    (void)row;
    return false;
}

void PhysicalScan::close(dreamdb::executor::ExecutionContext & context)
{
    // TODO: 实现关闭算子
    (void)context;
}

} // namespace dreamdb::planner::physical
