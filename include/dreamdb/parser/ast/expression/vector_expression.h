#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "dreamdb/parser/ast/expression/expression.h"

namespace dreamdb::parser::ast
{

/**
 * @brief 向量表达式节点
 * @details 用于表示 SQL 中的向量表达式，例如: [1.0, 2.0, 3.0]
 *          向量元素可以是任意表达式
 */
class AstVectorExpression : public AstExpression
{
public:
    AstVectorExpression(
        std::vector<std::unique_ptr<AstExpression>> elements,
        std::size_t line,
        std::size_t column
    );

    ~AstVectorExpression() noexcept override = default;

public:
    /**
     * @brief 创建向量表达式
     * @param elements 向量元素表达式列表
     * @param line 行号
     * @param column 列号
     * @return 向量表达式
     */
    static std::unique_ptr<AstVectorExpression> create(
        std::vector<std::unique_ptr<AstExpression>> elements,
        std::size_t line,
        std::size_t column
    );

public:
    /**
     * @brief 获取向量元素表达式
     * @param index 元素索引
     * @return 向量元素表达式
     */
    const AstExpression & element_at(std::size_t index) const noexcept;

    /**
     * @brief 获取向量元素数量
     * @return 向量元素数量
     */
    std::size_t element_count() const noexcept;

    /**
     * @brief 接受表达式访问者
     * @param visitor 表达式访问者
     */
    void accept(AstExpressionVisitor & visitor) const override;

private:
    std::vector<std::unique_ptr<AstExpression>> elements_;    // 向量元素表达式列表
};

} // namespace dreamdb::parser::ast
