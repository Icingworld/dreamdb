#include "dreamdb/planner/logical/operator/logical_update.h"

#include <cassert>

#include "dreamdb/binder/bound/expression/expression.h"

namespace dreamdb::planner::logical
{

LogicalUpdate::LogicalUpdate(dreamdb::common::collection_id_t collection_id, std::vector<LogicalUpdateItem> update_items)
    : LogicalOperator(LogicalOperatorType::Update)
    , collection_id_(collection_id)
    , update_items_(std::move(update_items))
{
    // 更新项不能为空
    assert(!update_items_.empty());

    // 注意：子节点是在创建后通过 add_child() 添加的
    // 因此不能在构造函数中检查 child_count()
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
