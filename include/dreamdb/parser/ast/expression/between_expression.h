#pragma once

#include <cstddef>
#include <memory>

#include "dreamdb/parser/ast/expression/expression.h"

namespace dreamdb::parser::ast
{

/**
 * @brief BETWEEN 表达式节点
 */
class AstBetweenExpression : public AstExpression
{
public:
    AstBetweenExpression(
        std::unique_ptr<AstExpression> left,
        std::unique_ptr<AstExpression> start,
        std::unique_ptr<AstExpression> end,
        bool is_not,
        std::size_t line,
        std::size_t column
    );

    ~AstBetweenExpression() noexcept override = default;

public:
    /**
     * @brief 创建 BETWEEN 表达式
     * @param left 左侧表达式
     * @param start 起始值表达式
     * @param end 结束值表达式
     * @param is_not 是否为 NOT BETWEEN
     * @param line 行号
     * @param column 列号
     * @return BETWEEN 表达式
     */
    static std::unique_ptr<AstBetweenExpression> create(
        std::unique_ptr<AstExpression> left,
        std::unique_ptr<AstExpression> start,
        std::unique_ptr<AstExpression> end,
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
     * @brief 获取起始值表达式
     * @return 起始值表达式引用
     */
    const AstExpression & start() const noexcept;

    /**
     * @brief 获取结束值表达式
     * @return 结束值表达式引用
     */
    const AstExpression & end() const noexcept;

    /**
     * @brief 是否为 NOT BETWEEN
     * @return 是否为 NOT BETWEEN
     */
    bool is_not() const noexcept;

    /**
     * @brief 接受表达式访问者
     * @param visitor 表达式访问者
     */
    void accept(AstExpressionVisitor & visitor) const override;

private:
    std::unique_ptr<AstExpression> left_;        // 左侧表达式
    std::unique_ptr<AstExpression> start_;       // 起始值表达式
    std::unique_ptr<AstExpression> end_;         // 结束值表达式
    bool is_not_;                                // 是否为 NOT BETWEEN
};

} // namespace dreamdb::parser::ast
