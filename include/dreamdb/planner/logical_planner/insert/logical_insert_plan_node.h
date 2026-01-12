#pragma once

#include <cstddef>
#include <vector>
#include <memory>

#include "dreamdb/planner/logical_planner/logical_plan_node.h"
#include "dreamdb/expression/expression.h"

namespace dreamdb
{

/**
 * @brief INSERT 逻辑计划节点
 */
class LogicalInsertPlanNode : public LogicalPlanNode
{
public:
    /**
     * @brief 插入项
     */
    struct InsertItem
    {
        std::unique_ptr<Expression> column_reference;   // 列引用表达式
        std::unique_ptr<Expression> value;             // 值表达式
    };

public:
    explicit LogicalInsertPlanNode(
        std::size_t collection_id,
        std::vector<InsertItem> insert_items
    );

    LogicalInsertPlanNode(const LogicalInsertPlanNode &) = delete;
    LogicalInsertPlanNode(LogicalInsertPlanNode &&) noexcept = default;
    LogicalInsertPlanNode & operator=(const LogicalInsertPlanNode &) = delete;
    LogicalInsertPlanNode & operator=(LogicalInsertPlanNode &&) noexcept = default;

    ~LogicalInsertPlanNode() override = default;

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

