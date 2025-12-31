#pragma once

#include <memory>

#include "dreamdb/expression/expression.h"

namespace dreamdb
{

/**
 * @brief NULL 表达式
 */
class NullExpression : public Expression
{
public:
    explicit NullExpression(std::unique_ptr<Expression> value, bool negated = false);

    NullExpression(const NullExpression &) noexcept = delete;

    NullExpression(NullExpression &&) noexcept = default;

    NullExpression & operator=(const NullExpression &) noexcept = delete;

    NullExpression & operator=(NullExpression &&) noexcept = default;

    ~NullExpression() noexcept = default;

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
     * @brief 是否为否定表达式
     * @return true 为否定表达式，false 为非否定表达式
     */
    bool is_negated() const noexcept;

private:
    std::unique_ptr<Expression> value_;    // 值表达式
    bool negated_;                         // 是否为否定表达式
};

} // namespace dreamdb
