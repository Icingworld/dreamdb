#pragma once

#include <memory>

#include "dreamdb/binder/bound/expression/expression.h"
#include "dreamdb/common/logical_type.h"

namespace dreamdb::binder::bound
{

class BoundExpressionVisitor;

/**
 * @brief 绑定后的 LIKE 表达式
 */
class BoundLikeExpression final : public BoundExpression
{
public:
    explicit BoundLikeExpression(
        std::unique_ptr<BoundExpression> left,
        std::unique_ptr<BoundExpression> pattern,
        dreamdb::common::LogicalType logical_type,
        bool is_not = false
    );

    ~BoundLikeExpression() noexcept override = default;

public:
    /**
     * @brief 获取左侧表达式
     * @return 左侧表达式
     */
    const BoundExpression & left() const noexcept;

    /**
     * @brief 获取模式表达式
     * @return 模式表达式
     */
    const BoundExpression & pattern() const noexcept;

    /**
     * @brief 是否为 NOT LIKE
     * @return 是否为 NOT LIKE
     */
    bool is_not() const noexcept;

    /**
     * @brief 接受表达式访问者
     * @param visitor 表达式访问者
     */
    void accept(BoundExpressionVisitor & visitor) const override;

private:
    std::unique_ptr<BoundExpression> left_;        // 左侧表达式
    std::unique_ptr<BoundExpression> pattern_;     // 模式表达式
    bool is_not_;                                  // 是否为 NOT LIKE
};

} // namespace dreamdb::binder::bound
