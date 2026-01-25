#pragma once

#include <string>
#include <optional>
#include <memory>

#include "dreamdb/planner/logical/logical_operator.h"

namespace dreamdb::binder::bound
{

class BoundExpression;

} // namespace dreamdb::binder::bound

namespace dreamdb::planner::logical
{

/**
 * @brief 聚合表达式项
 */
struct LogicalAggregateItem
{
    std::unique_ptr<dreamdb::binder::bound::BoundExpression> expression;  // 聚合表达式
    std::optional<std::string> alias;  // 别名
};

/**
 * @brief 逻辑聚合算子
 */
class LogicalAggregate : public LogicalOperator
{
public:
    LogicalAggregate(std::vector<std::unique_ptr<
        dreamdb::binder::bound::BoundExpression>
    > group_by, std::vector<LogicalAggregateItem> items);

    ~LogicalAggregate() noexcept override = default;

public:
    /**
     * @brief 获取分组表达式数量
     * @return 分组表达式数量
     */
    std::size_t group_by_count() const noexcept;

    /**
     * @brief 获取分组表达式
     * @param index 索引
     * @return 分组表达式
     */
    const dreamdb::binder::bound::BoundExpression & group_by_at(std::size_t index) const noexcept;

    /**
     * @brief 获取聚合表达式项的数量
     * @return 聚合表达式项的数量
     */
    std::size_t aggregate_items_count() const noexcept;

    /**
     * @brief 获取聚合表达式项
     * @param index 索引
     * @return 聚合表达式项
     */
    const LogicalAggregateItem & aggregate_item_at(std::size_t index) const noexcept;

private:
    std::vector<std::unique_ptr<
        dreamdb::binder::bound::BoundExpression>
    > group_by_;                                            // 分组表达式
    std::vector<LogicalAggregateItem> aggregate_items_;     // 聚合表达式项
};

} // namespace dreamdb::planner::logical
