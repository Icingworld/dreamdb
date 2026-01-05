#pragma once

#include <cstddef>
#include <memory>

#include "dreamdb/parser/ast/ast_expression_node.h"

namespace dreamdb
{

/**
 * @brief BETWEEN 表达式节点
 */
class AstBetweenExpressionNode : public AstExpressionNode
{
public:
    AstBetweenExpressionNode(std::size_t line = 0, std::size_t column = 0);

    AstBetweenExpressionNode(const AstBetweenExpressionNode &) = delete;

    AstBetweenExpressionNode(AstBetweenExpressionNode &&) noexcept = default;

    AstBetweenExpressionNode & operator=(const AstBetweenExpressionNode &) = delete;

    AstBetweenExpressionNode & operator=(AstBetweenExpressionNode &&) noexcept = default;

    ~AstBetweenExpressionNode() override = default;

public:
    /**
     * @brief 设置左侧表达式
     * @param left 左侧表达式
     */
    void set_left(std::unique_ptr<AstExpressionNode> left) noexcept;

    /**
     * @brief 设置起始值表达式
     * @param start 起始值表达式
     */
    void set_start(std::unique_ptr<AstExpressionNode> start) noexcept;

    /**
     * @brief 设置结束值表达式
     * @param end 结束值表达式
     */
    void set_end(std::unique_ptr<AstExpressionNode> end) noexcept;

    /**
     * @brief 设置是否为 NOT BETWEEN
     * @param is_not 是否为 NOT BETWEEN
     */
    void set_is_not(bool is_not) noexcept;

    /**
     * @brief 获取左侧表达式
     * @return 左侧表达式引用
     */
    const AstExpressionNode & get_left() const;

    /**
     * @brief 获取起始值表达式
     * @return 起始值表达式引用
     */
    const AstExpressionNode & get_start() const;

    /**
     * @brief 获取结束值表达式
     * @return 结束值表达式引用
     */
    const AstExpressionNode & get_end() const;

    /**
     * @brief 是否为 NOT BETWEEN
     * @return 是否为 NOT BETWEEN
     */
    bool is_not() const noexcept;

    /**
     * @brief 是否存在左侧表达式
     * @return 是否存在左侧表达式
     */
    bool has_left() const noexcept;

    /**
     * @brief 是否存在起始值表达式
     * @return 是否存在起始值表达式
     */
    bool has_start() const noexcept;

    /**
     * @brief 是否存在结束值表达式
     * @return 是否存在结束值表达式
     */
    bool has_end() const noexcept;

private:
    std::unique_ptr<AstExpressionNode> left_;        // 左侧表达式
    std::unique_ptr<AstExpressionNode> start_;       // 起始值表达式
    std::unique_ptr<AstExpressionNode> end_;         // 结束值表达式
    bool is_not_;                                    // 是否为 NOT BETWEEN
};

} // namespace dreamdb
