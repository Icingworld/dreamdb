#pragma once

#include <cstddef>
#include <optional>

#include "dreamdb/parser/ast/ast_expression_node.h"

namespace dreamdb
{

/**
 * @brief 参数表达式节点
 */
class AstParameterExpressionNode : public AstExpressionNode
{
public:
    AstParameterExpressionNode(std::size_t line = 0, std::size_t column = 0);

    AstParameterExpressionNode(const AstParameterExpressionNode &) = delete;

    AstParameterExpressionNode(AstParameterExpressionNode &&) noexcept = default;

    AstParameterExpressionNode & operator=(const AstParameterExpressionNode &) = delete;

    AstParameterExpressionNode & operator=(AstParameterExpressionNode &&) noexcept = default;

    ~AstParameterExpressionNode() override = default;

public:
    /**
     * @brief 设置参数索引
     * @param parameter_index 参数索引
     */
    void set_parameter_index(std::size_t parameter_index);

    /**
     * @brief 获取参数索引
     * @return 参数索引
     */
    std::size_t get_parameter_index() const noexcept;

    /**
     * @brief 是否存在参数索引
     * @return 是否存在参数索引
     */
    bool has_parameter_index() const noexcept;

private:
    std::optional<std::size_t> parameter_index_;  // 参数索引
};

} // namespace dreamdb
