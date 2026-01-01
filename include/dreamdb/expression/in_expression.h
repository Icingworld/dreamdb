#pragma once

#include <memory>
#include <vector>

#include "dreamdb/expression/expression.h"

namespace dreamdb
{

/**
 * @brief IN 表达式
 */
class InExpression : public Expression
{
public:
    explicit InExpression(std::unique_ptr<Expression> value, std::vector<std::unique_ptr<Expression>> list, bool negated = false);

    InExpression(const InExpression &) noexcept = delete;

    InExpression(InExpression &&) noexcept = default;

    InExpression & operator=(const InExpression &) noexcept = delete;

    InExpression & operator=(InExpression &&) noexcept = default;

    ~InExpression() noexcept = default;

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
     * @brief 获取列表表达式列表
     * @return 列表表达式列表
     */
    const std::vector<std::unique_ptr<Expression>> & get_list() const noexcept;

    /**
     * @brief 获取可变列表表达式列表
     * @return 可变列表表达式列表
     * @details 该接口供 Optimizer 使用
     */
    std::vector<std::unique_ptr<Expression>> & get_mutable_list() noexcept;

    /**
     * @brief 是否为否定表达式
     * @return true 为否定表达式，false 为非否定表达式
     */
    bool is_negated() const noexcept;

private:
    std::unique_ptr<Expression> value_;                // 值表达式
    std::vector<std::unique_ptr<Expression>> list_;    // 列表表达式列表
    bool negated_;                                     // 是否为否定表达式
};

} // namespace dreamdb
