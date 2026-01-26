#pragma once

#include <memory>

#include "dreamdb/planner/physical/physical_operator.h"

namespace dreamdb::planner::physical
{

/**
 * @brief 一元算子
 * @details 一元算子有一个子算子，封装了 open 和 close 方法，避免子算子重复实现
 */
class PhysicalUnaryOperator : public PhysicalOperator
{
protected:
    explicit PhysicalUnaryOperator(std::unique_ptr<PhysicalOperator> child);

public:
    virtual ~PhysicalUnaryOperator() override = default;

public:
    /**
     * @brief 打开算子
     * @param context 执行上下文
     */
    void open(ExecutionContext & context) override;

    /**
     * @brief 关闭算子
     * @param context 执行上下文
     */
    void close(ExecutionContext & context) override;

protected:
    std::unique_ptr<PhysicalOperator> child_;    // 子算子
};

} // namespace dreamdb::planner::physical
