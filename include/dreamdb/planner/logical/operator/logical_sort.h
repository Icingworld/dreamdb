#pragma once

#include "dreamdb/planner/logical/logical_operator.h"
#include "dreamdb/common/type.h"

namespace dreamdb::binder::bound
{

class BoundExpression;

} // namespace dreamdb::binder::bound

namespace dreamdb::planner::logical
{

/**
 * @brief 逻辑排序项
 * @details 用于表示排序的项
 */
struct LogicalSortItem
{
    std::unique_ptr<dreamdb::binder::bound::BoundExpression> expression;   // 排序表达式
    dreamdb::common::Direction direction;                                  // 排序方向
};

/**
 * @brief 逻辑排序算子
 * @details 用于排序的算子
 */
class LogicalSort final : public LogicalOperator
{
public:
    LogicalSort(std::vector<LogicalSortItem> sort_items);

    ~LogicalSort() noexcept override = default;

public:
    /**
     * @brief 获取排序项数量
     * @return 排序项数量
     */
    std::size_t sort_item_count() const noexcept;

    /**
     * @brief 获取排序项
     * @param index 排序项索引
     * @return 排序项
     */
    const LogicalSortItem & sort_item_at(std::size_t index) const noexcept;

private:
    std::vector<LogicalSortItem> sort_items_;    // 排序项列表
};

} // namespace dreamdb::planner::logical
