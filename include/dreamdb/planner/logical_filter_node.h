#pragma once

#include <memory>

#include "dreamdb/planner/logical_plan_node.h"
#include "dreamdb/expression/expression.h"

namespace dreamdb
{

/**
 * @brief 过滤节点
 */
class LogicalFilterNode : public LogicalPlanNode
{
public:
    explicit LogicalFilterNode(std::unique_ptr<Expression> predicate);

    LogicalFilterNode(const LogicalFilterNode &) = delete;

    LogicalFilterNode(LogicalFilterNode &&) noexcept = default;

    LogicalFilterNode & operator=(const LogicalFilterNode &) = delete;

    LogicalFilterNode & operator=(LogicalFilterNode &&) noexcept = default;

    ~LogicalFilterNode() override = default;

public:
    /**
     * @brief 获取谓词
     * @return 谓词
     */
    const Expression & get_predicate() const noexcept;

    /**
     * @brief 获取可变谓词
     * @return 谓词
     */
    Expression & get_mutable_predicate() noexcept;

private:
    std::unique_ptr<Expression> predicate_;   // 谓词
};

} // namespace dreamdb
