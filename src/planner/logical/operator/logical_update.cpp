#include "dreamdb/planner/logical/operator/logical_update.h"

#include <cassert>

namespace dreamdb::planner::logical
{

LogicalUpdate::LogicalUpdate(dreamdb::common::collection_id_t collection_id, std::vector<LogicalUpdateItem> update_items)
    : LogicalOperator(LogicalOperatorType::Update)
    , collection_id_(collection_id)
    , update_items_(std::move(update_items))
{
    // 更新项不能为空
    assert(!update_items_.empty());

    // Update 算子必须有且只有一个子算子
    assert(child_count() == 1);
}

dreamdb::common::collection_id_t LogicalUpdate::collection_id() const noexcept
{
    return collection_id_;
}

std::size_t LogicalUpdate::update_item_count() const noexcept
{
    return update_items_.size();
}

const LogicalUpdateItem & LogicalUpdate::update_item_at(std::size_t index) const noexcept
{
    return update_items_[index];
}

} // namespace dreamdb::planner::logical