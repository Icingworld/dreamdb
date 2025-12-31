#pragma once

#include <memory>

#include "dreamdb/expression/expression.h"

namespace dreamdb
{

/**
 * @brief LIKE 表达式
 */
class LikeExpression : public Expression
{
public:
    explicit LikeExpression(std::unique_ptr<Expression> value, std::unique_ptr<Expression> pattern, bool negated = false);

    LikeExpression(const LikeExpression &) noexcept = delete;

    LikeExpression(LikeExpression &&) noexcept = default;

    LikeExpression & operator=(const LikeExpression &) noexcept = delete;

    LikeExpression & operator=(LikeExpression &&) noexcept = default;

    ~LikeExpression() noexcept = default;

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
     * @brief 获取模式表达式
     * @return 模式表达式
     */
    const Expression & get_pattern() const noexcept;

    /**
     * @brief 获取可变模式表达式
     * @return 可变模式表达式
     * @details 该接口供 Optimizer 使用
     */
    Expression & get_mutable_pattern() noexcept;

    /**
     * @brief 是否为否定表达式
     * @return true 为否定表达式，false 为非否定表达式
     */
    
    bool is_negated() const noexcept;

private:
    std::unique_ptr<Expression> value_;    // 值表达式
    std::unique_ptr<Expression> pattern_;  // 模式表达式
    bool negated_;                         // 是否为否定表达式
};

} // namespace dreamdb
