#include "dreamdb/planner/logical/operator/logical_delete.h"

#include <cassert>

namespace dreamdb::planner::logical
{

LogicalDelete::LogicalDelete(dreamdb::common::collection_id_t collection_id)
    : LogicalOperator(LogicalOperatorType::Delete)
    , collection_id_(collection_id)
{
    // Delete 算子必须有且只有一个子算子
    assert(child_count() == 1);
}

dreamdb::common::collection_id_t LogicalDelete::collection_id() const noexcept
{
    return collection_id_;
}

} // namespace dreamdb::planner::logical
