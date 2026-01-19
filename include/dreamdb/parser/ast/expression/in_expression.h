#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "dreamdb/parser/ast/expression/expression.h"

namespace dreamdb::parser::ast
{

/**
 * @brief IN 表达式节点
 */
class AstInExpression final : public AstExpression
{
public:
    AstInExpression(
        std::unique_ptr<AstExpression> left,
        std::vector<std::unique_ptr<AstExpression>> values,
        bool is_not,
        std::size_t line,
        std::size_t column
    );

    ~AstInExpression() noexcept override = default;

public:
    /**
     * @brief 创建 IN 表达式
     * @param left 左侧表达式
     * @param values 值表达式列表
     * @param is_not 是否为 NOT IN
     * @param line 行号
     * @param column 列号
     * @return IN 表达式
     */
    static std::unique_ptr<AstInExpression> create(
        std::unique_ptr<AstExpression> left,
        std::vector<std::unique_ptr<AstExpression>> values,
        bool is_not,
        std::size_t line,
        std::size_t column
    );

public:
    /**
     * @brief 获取左侧表达式
     * @return 左侧表达式引用
     */
    const AstExpression & left() const noexcept;

    /**
     * @brief 获取指定索引的值表达式
     * @param index 值表达式索引
     * @return 值表达式引用
     */
    const AstExpression & value_at(std::size_t index) const noexcept;

    /**
     * @brief 是否为 NOT IN
     * @return 是否为 NOT IN
     */
    bool is_not() const noexcept;

    /**
     * @brief 获取值表达式数量
     * @return 值表达式数量
     */
    std::size_t value_count() const noexcept;

    /**
     * @brief 接受表达式访问者
     * @param visitor 表达式访问者
     */
    void accept(AstExpressionVisitor & visitor) const override;

private:
    std::unique_ptr<AstExpression> left_;                   // 左侧表达式
    std::vector<std::unique_ptr<AstExpression>> values_;    // 值表达式列表
    bool is_not_;                                           // 是否为 NOT IN
};

} // namespace dreamdb::parser::ast
