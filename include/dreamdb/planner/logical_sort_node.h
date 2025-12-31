#pragma once

#include "dreamdb/planner/logical_plan_node.h"
#include "dreamdb/expression/expression.h"

namespace dreamdb
{

/**
 * @brief 排序项
 */
struct SortItem
{
    std::unique_ptr<Expression> expression;    // 排序表达式
    bool ascending;                            // 是否升序
    bool nulls_first;                          // 是否将 NULL 值排在前面
};

/**
 * @brief 排序节点
 */
class LogicalSortNode : public LogicalPlanNode
{
public:
    explicit LogicalSortNode(std::vector<SortItem> sort_items);

    LogicalSortNode(const LogicalSortNode &) noexcept = delete;

    LogicalSortNode(LogicalSortNode &&) noexcept = default;

    LogicalSortNode & operator=(const LogicalSortNode &) noexcept = delete;

    LogicalSortNode & operator=(LogicalSortNode &&) noexcept = default;

    ~LogicalSortNode() noexcept = default;

public:
    /**
     * @brief 获取排序项列表
     * @return 排序项列表
     */
    const std::vector<SortItem> & get_sort_items() const noexcept;

    /**
     * @brief 获取可变排序项列表
     * @return 排序项列表
     */
    std::vector<SortItem> & get_mutable_sort_items() noexcept;  

private:
    std::vector<SortItem> sort_items_;    // 排序项列表
};

} // namespace dreamdb
