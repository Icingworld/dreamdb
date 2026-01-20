#pragma once

#include <memory>

#include "dreamdb/binder/bound/expression/expression.h"
#include "dreamdb/common/logical_type.h"

namespace dreamdb::binder::bound
{

/**
 * @brief 绑定后的 BETWEEN 表达式
 */
class BoundBetweenExpression final : public BoundExpression
{
public:
    explicit BoundBetweenExpression(
        std::unique_ptr<BoundExpression> left,
        std::unique_ptr<BoundExpression> start,
        std::unique_ptr<BoundExpression> end,
        dreamdb::common::LogicalType logical_type,
        bool is_not = false
    );

    ~BoundBetweenExpression() noexcept override = default;

public:
    /**
     * @brief 获取左侧表达式
     * @return 左侧表达式
     */
    const BoundExpression & left() const noexcept;

    /**
     * @brief 获取起始值表达式
     * @return 起始值表达式
     */
    const BoundExpression & start() const noexcept;

    /**
     * @brief 获取结束值表达式
     * @return 结束值表达式
     */
    const BoundExpression & end() const noexcept;

    /**
     * @brief 是否为 NOT BETWEEN
     * @return 是否为 NOT BETWEEN
     */
    bool is_not() const noexcept;

private:
    std::unique_ptr<BoundExpression> left_;        // 左侧表达式
    std::unique_ptr<BoundExpression> start_;       // 起始值表达式
    std::unique_ptr<BoundExpression> end_;         // 结束值表达式
    bool is_not_;                                  // 是否为 NOT BETWEEN
};

} // namespace dreamdb::binder::bound
