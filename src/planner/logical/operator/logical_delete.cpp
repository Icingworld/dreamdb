#include "dreamdb/planner/logical/operator/logical_delete.h"

#include <cassert>

namespace dreamdb::planner::logical
{

LogicalDelete::LogicalDelete(dreamdb::common::collection_id_t collection_id)
    : LogicalOperator(LogicalOperatorType::Delete)
    , collection_id_(collection_id)
{
    // 注意：子节点是在创建后通过 add_child() 添加的
    // 因此不能在构造函数中检查 child_count()
}

dreamdb::common::collection_id_t LogicalDelete::collection_id() const noexcept
{
    return collection_id_;
}

} // namespace dreamdb::planner::logical
