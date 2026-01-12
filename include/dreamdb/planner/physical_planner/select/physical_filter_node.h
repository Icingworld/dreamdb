#pragma once

#include <memory>

#include "dreamdb/planner/physical_planner/select/physical_select_plan_node.h"
#include "dreamdb/expression/expression.h"

namespace dreamdb
{

/**
 * @brief 过滤物理操作符
 */
class PhysicalFilterNode : public PhysicalSelectPlanNode
{
public:
    explicit PhysicalFilterNode(std::unique_ptr<Expression> predicate);

    PhysicalFilterNode(const PhysicalFilterNode &) = delete;
    PhysicalFilterNode(PhysicalFilterNode &&) noexcept = default;
    PhysicalFilterNode & operator=(const PhysicalFilterNode &) = delete;
    PhysicalFilterNode & operator=(PhysicalFilterNode &&) noexcept = default;

    ~PhysicalFilterNode() override = default;

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
