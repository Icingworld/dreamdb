#pragma once

#include <cstddef>
#include <memory>

#include "dreamdb/planner/logical_planner/logical_plan_node.h"
#include "dreamdb/expression/expression.h"

namespace dreamdb
{

/**
 * @brief DELETE 逻辑计划节点
 */
class LogicalDeletePlanNode : public LogicalPlanNode
{
public:
    explicit LogicalDeletePlanNode(
        std::size_t collection_id,
        std::unique_ptr<Expression> where_clause
    );

    LogicalDeletePlanNode(const LogicalDeletePlanNode &) = delete;
    LogicalDeletePlanNode(LogicalDeletePlanNode &&) noexcept = default;
    LogicalDeletePlanNode & operator=(const LogicalDeletePlanNode &) = delete;
    LogicalDeletePlanNode & operator=(LogicalDeletePlanNode &&) noexcept = default;

    ~LogicalDeletePlanNode() override = default;

public:
    /**
     * @brief 获取集合 ID
     * @return 集合 ID
     */
    std::size_t get_collection_id() const noexcept;

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
    std::unique_ptr<Expression> where_clause_; // WHERE 子句
};

} // namespace dreamdb
