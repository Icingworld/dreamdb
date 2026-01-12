#pragma once

#include <cstddef>
#include <vector>
#include <memory>

#include "dreamdb/planner/logical_planner/logical_plan_node.h"
#include "dreamdb/expression/expression.h"

namespace dreamdb
{

/**
 * @brief UPDATE 逻辑计划节点
 */
class LogicalUpdatePlanNode : public LogicalPlanNode
{
public:
    /**
     * @brief 更新项
     */
    struct UpdateItem
    {
        std::unique_ptr<Expression> column_reference;   // 列引用表达式
        std::unique_ptr<Expression> value;             // 值表达式
    };

public:
    explicit LogicalUpdatePlanNode(
        std::size_t collection_id,
        std::vector<UpdateItem> update_items,
        std::unique_ptr<Expression> where_clause
    );

    LogicalUpdatePlanNode(const LogicalUpdatePlanNode &) = delete;
    LogicalUpdatePlanNode(LogicalUpdatePlanNode &&) noexcept = default;
    LogicalUpdatePlanNode & operator=(const LogicalUpdatePlanNode &) = delete;
    LogicalUpdatePlanNode & operator=(LogicalUpdatePlanNode &&) noexcept = default;

    ~LogicalUpdatePlanNode() override = default;

public:
    /**
     * @brief 获取集合 ID
     * @return 集合 ID
     */
    std::size_t get_collection_id() const noexcept;

    /**
     * @brief 获取更新项列表
     * @return 更新项列表
     */
    const std::vector<UpdateItem> & get_update_items() const noexcept;

    /**
     * @brief 获取 WHERE 子句
     * @return WHERE 子句，如果不存在返回 nullptr
     */
    const Expression * get_where_clause() const noexcept;

    /**
     * @brief 获取可变 WHERE 子句
     * @return WHERE 子句
     */
    Expression * get_mutable_where_clause() noexcept;

private:
    std::size_t collection_id_;                // 集合 ID
    std::vector<UpdateItem> update_items_;     // 更新项列表
    std::unique_ptr<Expression> where_clause_; // WHERE 子句
};

} // namespace dreamdb
