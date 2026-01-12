#pragma once

#include <cstddef>
#include <vector>
#include <memory>

#include "dreamdb/planner/physical_planner/physical_plan_node.h"
#include "dreamdb/expression/expression.h"

namespace dreamdb
{

/**
 * @brief INSERT 物理计划节点
 * @details INSERT 操作直接插入值到集合中
 */
class PhysicalInsertPlanNode : public PhysicalPlanNode
{
public:
    /**
     * @brief 插入项
     */
    struct InsertItem
    {
        std::unique_ptr<Expression> column_reference;   // 列引用表达式
        std::unique_ptr<Expression> value;              // 值表达式
    };

public:
    explicit PhysicalInsertPlanNode(
        std::size_t collection_id,
        std::vector<InsertItem> insert_items
    );

    PhysicalInsertPlanNode(const PhysicalInsertPlanNode &) = delete;
    PhysicalInsertPlanNode(PhysicalInsertPlanNode &&) noexcept = default;
    PhysicalInsertPlanNode & operator=(const PhysicalInsertPlanNode &) = delete;
    PhysicalInsertPlanNode & operator=(PhysicalInsertPlanNode &&) noexcept = default;

    ~PhysicalInsertPlanNode() override = default;

public:
    /**
     * @brief 获取集合 ID
     * @return 集合 ID
     */
    std::size_t get_collection_id() const noexcept;

    /**
     * @brief 获取插入项列表
     * @return 插入项列表
     */
    const std::vector<InsertItem> & get_insert_items() const noexcept;

private:
    std::size_t collection_id_;                // 集合 ID
    std::vector<InsertItem> insert_items_;     // 插入项列表
};

} // namespace dreamdb
