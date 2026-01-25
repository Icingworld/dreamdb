#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "dreamdb/binder/bound/expression/expression.h"
#include "dreamdb/common/logical_type.h"

namespace dreamdb::binder::bound
{

class BoundExpressionVisitor;

/**
 * @brief 绑定后的 IN 表达式
 */
class BoundInExpression final : public BoundExpression
{
public:
    explicit BoundInExpression(
        std::unique_ptr<BoundExpression> left,
        std::vector<std::unique_ptr<BoundExpression>> values,
        dreamdb::common::LogicalType logical_type,
        bool is_not = false
    );

    ~BoundInExpression() noexcept override = default;

public:
    /**
     * @brief 获取左侧表达式
     * @return 左侧表达式
     */
    const BoundExpression & left() const noexcept;

    /**
     * @brief 获取值表达式数量
     * @return 值表达式数量
     */
    std::size_t value_count() const noexcept;

    /**
     * @brief 获取指定索引的值表达式
     * @param index 值表达式索引
     * @return 值表达式
     */
    const BoundExpression & value_at(std::size_t index) const noexcept;

    /**
     * @brief 是否为 NOT IN
     * @return 是否为 NOT IN
     */
    bool is_not() const noexcept;

    /**
     * @brief 接受表达式访问者
     * @param visitor 表达式访问者
     */
    void accept(BoundExpressionVisitor & visitor) const override;

    /**
     * @brief 克隆表达式
     * @return 克隆后的表达式
     */
    std::unique_ptr<BoundExpression> clone() const override;

private:
    std::unique_ptr<BoundExpression> left_;                             // 左侧表达式
    std::vector<std::unique_ptr<BoundExpression>> values_;              // 值表达式列表
    bool is_not_;                                                       // 是否为 NOT IN
};

} // namespace dreamdb::binder::bound
