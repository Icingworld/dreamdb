#pragma once

#include <memory>

#include "dreamdb/expression/expression.h"

namespace dreamdb
{
    
/**
 * @brief BETWEEN 表达式
 */
class BetweenExpression : public Expression
{
public:
    explicit BetweenExpression(std::unique_ptr<Expression> value, std::unique_ptr<Expression> min, std::unique_ptr<Expression> max, bool negated = false);

    BetweenExpression(const BetweenExpression &) noexcept = delete;

    BetweenExpression(BetweenExpression &&) noexcept = default;

    BetweenExpression & operator=(const BetweenExpression &) noexcept = delete;

    BetweenExpression & operator=(BetweenExpression &&) noexcept = default;

    ~BetweenExpression() noexcept = default;

public:
    /**
     * @brief 获取值表达式
     * @return 值表达式
     */
    const Expression & get_value() const noexcept;

    /**
     * @brief 获取可变值表达式
     * @return 可变值表达式
     * @details 该接口供 Optimizer 使用
     */
    Expression & get_mutable_value() noexcept;

    /**
     * @brief 获取最小值表达式
     * @return 最小值表达式
     */    
    const Expression & get_min() const noexcept;

    /**
     * @brief 获取可变最小值表达式
     * @return 可变最小值表达式
     * @details 该接口供 Optimizer 使用
     */
    Expression & get_mutable_min() noexcept;
    
    /**
     * @brief 获取最大值表达式
     * @return 最大值表达式
     */
    const Expression & get_max() const noexcept;

    /**
     * @brief 获取可变最大值表达式
     * @return 可变最大值表达式
     * @details 该接口供 Optimizer 使用
     */
    Expression & get_mutable_max() noexcept;
    
    /**
     * @brief 是否为否定表达式
     * @return true 为否定表达式，false 为非否定表达式
     */
    bool is_negated() const noexcept;

private:
    std::unique_ptr<Expression> value_;    // 值表达式
    std::unique_ptr<Expression> min_;      // 最小值表达式
    std::unique_ptr<Expression> max_;      // 最大值表达式
    bool negated_;                         // 是否为否定表达式
};

} // namespace dreamdb
