#pragma once

#include "dreamdb/planner/physical/operator/physical_unary_operator.h"
#include "dreamdb/planner/physical/physical_operator.h"

namespace dreamdb::binder::bound
{

class BoundExpression;

} // namespace dreamdb::binder::bound

namespace dreamdb::planner::physical
{

/**
 * @brief 物理过滤算子
 */
class PhysicalFilter final : public PhysicalUnaryOperator
{
public:
    explicit PhysicalFilter(
        std::unique_ptr<PhysicalOperator> child,
        std::unique_ptr<dreamdb::binder::bound::BoundExpression> predicate
    );

    ~PhysicalFilter() override = default;

public:
    /**
     * @brief 打开算子
     * @param context 执行上下文
     */
    void open(dreamdb::executor::ExecutionContext & context) override;

    /**
     * @brief 获取下一行
     * @param context 执行上下文
     * @param row 当前行
     * @return 是否还有下一行
     */
    bool next(dreamdb::executor::ExecutionContext & context, dreamdb::storage::Row & row) override;

    /**
     * @brief 关闭算子
     * @param context 执行上下文
     */
    void close(dreamdb::executor::ExecutionContext & context) override;

private:
    /**
     * @brief 评估过滤条件
     * @param context 执行上下文
     * @param row 当前行
     * @return 是否满足过滤条件
     */
    bool evaluate(dreamdb::executor::ExecutionContext & context, const dreamdb::storage::Row & row) const;

private:
    std::unique_ptr<dreamdb::binder::bound::BoundExpression> predicate_;  // 过滤条件
};

} // namespace dreamdb::planner::physical
