#pragma once

#include <cstddef>
#include <memory>

#include "dreamdb/parser/ast/expression/expression.h"

namespace dreamdb::parser::ast
{

/**
 * @brief LIKE 表达式节点
 */
class AstLikeExpression : public AstExpression
{
public:
    AstLikeExpression(
        std::unique_ptr<AstExpression> left,
        std::unique_ptr<AstExpression> pattern,
        bool is_not,
        std::size_t line,
        std::size_t column
    );

    ~AstLikeExpression() noexcept override = default;

public:
    /**
     * @brief 创建 LIKE 表达式
     * @param left 左侧表达式
     * @param pattern 模式表达式
     * @param is_not 是否为 NOT LIKE
     * @param line 行号
     * @param column 列号
     * @return LIKE 表达式
     */
    static std::unique_ptr<AstLikeExpression> create(
        std::unique_ptr<AstExpression> left,
        std::unique_ptr<AstExpression> pattern,
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
     * @brief 获取模式表达式
     * @return 模式表达式引用
     */
    const AstExpression & pattern() const noexcept;

    /**
     * @brief 是否为 NOT LIKE
     * @return 是否为 NOT LIKE
     */
    bool is_not() const noexcept;

    /**
     * @brief 接受表达式访问者
     * @param visitor 表达式访问者
     */
    void accept(AstExpressionVisitor & visitor) const override;

private:
    std::unique_ptr<AstExpression> left_;        // 左侧表达式
    std::unique_ptr<AstExpression> pattern_;     // 模式表达式
    bool is_not_;                                // 是否为 NOT LIKE
};

} // namespace dreamdb::parser::ast
