#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "dreamdb/parser/ast/ast_expression_node.h"

namespace dreamdb
{

/**
 * @brief IN 表达式节点
 */
class AstInExpressionNode : public AstExpressionNode
{
public:
    AstInExpressionNode(std::size_t line = 0, std::size_t column = 0);

    AstInExpressionNode(const AstInExpressionNode &) = delete;

    AstInExpressionNode(AstInExpressionNode &&) noexcept = default;

    AstInExpressionNode & operator=(const AstInExpressionNode &) = delete;

    AstInExpressionNode & operator=(AstInExpressionNode &&) noexcept = default;

    ~AstInExpressionNode() override = default;

public:
    /**
     * @brief 设置左侧表达式
     * @param left 左侧表达式
     */
    void set_left(std::unique_ptr<AstExpressionNode> left) noexcept;

    /**
     * @brief 添加值表达式到列表
     * @param value 值表达式
     */
    void add_value(std::unique_ptr<AstExpressionNode> value) noexcept;

    /**
     * @brief 设置是否为 NOT IN
     * @param is_not 是否为 NOT IN
     */
    void set_is_not(bool is_not) noexcept;

    /**
     * @brief 获取左侧表达式
     * @return 左侧表达式引用
     */
    const AstExpressionNode & get_left() const;

    /**
     * @brief 获取值表达式列表
     * @return 值表达式列表的引用
     */
    const std::vector<std::unique_ptr<AstExpressionNode>> & get_values() const;

    /**
     * @brief 是否为 NOT IN
     * @return 是否为 NOT IN
     */
    bool is_not() const noexcept;

    /**
     * @brief 是否存在左侧表达式
     * @return 是否存在左侧表达式
     */
    bool has_left() const noexcept;

    /**
     * @brief 是否存在值列表
     * @return 是否存在值列表
     */
    bool has_values() const noexcept;

private:
    std::unique_ptr<AstExpressionNode> left_;                                    // 左侧表达式
    std::vector<std::unique_ptr<AstExpressionNode>> values_;                     // 值表达式列表
    bool is_not_;                                                                // 是否为 NOT IN
};

} // namespace dreamdb
