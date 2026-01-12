#pragma once

#include <vector>
#include <memory>

#include "dreamdb/planner/physical_planner/select/physical_select_plan_node.h"
#include "dreamdb/expression/expression.h"

namespace dreamdb
{

/**
 * @brief 排序项
 */
struct PhysicalSortItem
{
    std::unique_ptr<Expression> expression;    // 排序表达式
    bool ascending;                            // 是否升序
    bool nulls_first;                          // 是否将 NULL 值排在前面
};

/**
 * @brief 排序物理操作符
 */
class PhysicalSortNode : public PhysicalSelectPlanNode
{
public:
    explicit PhysicalSortNode(std::vector<PhysicalSortItem> sort_items);

    PhysicalSortNode(const PhysicalSortNode &) = delete;
    PhysicalSortNode(PhysicalSortNode &&) noexcept = default;
    PhysicalSortNode & operator=(const PhysicalSortNode &) = delete;
    PhysicalSortNode & operator=(PhysicalSortNode &&) noexcept = default;

    ~PhysicalSortNode() override = default;

public:
    /**
     * @brief 获取排序项列表
     * @return 排序项列表
     */
    const std::vector<PhysicalSortItem> & get_sort_items() const noexcept;

    /**
     * @brief 获取可变排序项列表
     * @return 排序项列表
     */
    std::vector<PhysicalSortItem> & get_mutable_sort_items() noexcept;

private:
    std::vector<PhysicalSortItem> sort_items_;    // 排序项列表
};

} // namespace dreamdb
