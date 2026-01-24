#pragma once

#include "dreamdb/planner/logical/logical_operator.h"
#include "dreamdb/common/ids.h"

namespace dreamdb::binder::bound
{

class BoundExpression;

} // namespace dreamdb::binder::bound

namespace dreamdb::planner::logical
{

/**
 * @brief 逻辑更新项
 */
struct LogicalUpdateItem
{
    dreamdb::common::column_id_t column_id;  // 列 ID
    std::unique_ptr<dreamdb::binder::bound::BoundExpression> expression;  // 更新值表达式
};

/**
 * @brief 逻辑更新算子
 */
class LogicalUpdate : public LogicalOperator
{
public:
    LogicalUpdate(dreamdb::common::collection_id_t collection_id, std::vector<LogicalUpdateItem> update_items);

    ~LogicalUpdate() noexcept override = default;

public:
    /**
     * @brief 获取集合 ID
     * @return 集合 ID
     */
    dreamdb::common::collection_id_t collection_id() const noexcept;

    /**
     * @brief 获取更新表达式项的数量
     * @return 更新表达式项的数量
     */
    std::size_t update_item_count() const noexcept;

    /**
     * @brief 获取更新表达式项
     * @param index 索引
     * @return 更新表达式项
     */
    const LogicalUpdateItem & update_item_at(std::size_t index) const noexcept;

private:
    dreamdb::common::collection_id_t collection_id_;    // 集合 ID
    std::vector<LogicalUpdateItem> update_items_;       // 更新表达式项
};

} // namespace dreamdb::planner::logical