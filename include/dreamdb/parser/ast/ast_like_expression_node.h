#pragma once

#include <cstddef>
#include <memory>

#include "dreamdb/parser/ast/ast_expression_node.h"

namespace dreamdb
{

/**
 * @brief LIKE 表达式节点
 */
class AstLikeExpressionNode : public AstExpressionNode
{
public:
    AstLikeExpressionNode(std::size_t line = 0, std::size_t column = 0);

    AstLikeExpressionNode(const AstLikeExpressionNode &) = delete;

    AstLikeExpressionNode(AstLikeExpressionNode &&) noexcept = default;

    AstLikeExpressionNode & operator=(const AstLikeExpressionNode &) = delete;

    AstLikeExpressionNode & operator=(AstLikeExpressionNode &&) noexcept = default;

    ~AstLikeExpressionNode() override = default;

public:
    /**
     * @brief 设置左侧表达式
     * @param left 左侧表达式
     */
    void set_left(std::unique_ptr<AstExpressionNode> left) noexcept;

    /**
     * @brief 设置模式表达式
     * @param pattern 模式表达式
     */
    void set_pattern(std::unique_ptr<AstExpressionNode> pattern) noexcept;

    /**
     * @brief 设置是否为 NOT LIKE
     * @param is_not 是否为 NOT LIKE
     */
    void set_is_not(bool is_not) noexcept;

    /**
     * @brief 获取左侧表达式
     * @return 左侧表达式引用
     */
    const AstExpressionNode & get_left() const;

    /**
     * @brief 获取模式表达式
     * @return 模式表达式引用
     */
    const AstExpressionNode & get_pattern() const;

    /**
     * @brief 是否为 NOT LIKE
     * @return 是否为 NOT LIKE
     */
    bool is_not() const noexcept;

    /**
     * @brief 是否存在左侧表达式
     * @return 是否存在左侧表达式
     */
    bool has_left() const noexcept;

    /**
     * @brief 是否存在模式表达式
     * @return 是否存在模式表达式
     */
    bool has_pattern() const noexcept;

private:
    std::unique_ptr<AstExpressionNode> left_;        // 左侧表达式
    std::unique_ptr<AstExpressionNode> pattern_;     // 模式表达式
    bool is_not_;                                    // 是否为 NOT LIKE
};

} // namespace dreamdb
