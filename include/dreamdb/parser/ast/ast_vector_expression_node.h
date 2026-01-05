#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "dreamdb/parser/ast/ast_expression_node.h"

namespace dreamdb
{

/**
 * @brief 向量表达式节点
 * @details 用于表示 SQL 中的向量表达式，例如: [1.0, 2.0, 3.0]
 *          向量元素可以是任意表达式
 */
class AstVectorExpressionNode : public AstExpressionNode
{
public:
    AstVectorExpressionNode(std::size_t line = 0, std::size_t column = 0);

    AstVectorExpressionNode(const AstVectorExpressionNode &) = delete;

    AstVectorExpressionNode(AstVectorExpressionNode &&) noexcept = default;

    AstVectorExpressionNode & operator=(const AstVectorExpressionNode &) = delete;

    AstVectorExpressionNode & operator=(AstVectorExpressionNode &&) noexcept = default;

    ~AstVectorExpressionNode() override = default;

public:
    /**
     * @brief 添加元素表达式到向量
     * @param element 元素表达式
     */
    void add_element(std::unique_ptr<AstExpressionNode> element) noexcept;

    /**
     * @brief 获取向量元素表达式列表
     * @return 向量元素表达式列表的引用
     */
    const std::vector<std::unique_ptr<AstExpressionNode>> & get_elements() const noexcept;

    /**
     * @brief 是否存在元素
     * @return 是否存在元素
     */
    bool has_elements() const noexcept;

    /**
     * @brief 获取向量元素数量
     * @return 向量元素数量
     */
    std::size_t get_size() const noexcept;

private:
    std::vector<std::unique_ptr<AstExpressionNode>> elements_;    // 向量元素表达式列表
};

} // namespace dreamdb
