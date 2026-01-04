#pragma once

#include <cstddef>
#include <vector>
#include <string>
#include <memory>
#include <optional>

#include "dreamdb/parser/ast/ast_expression_node.h"

namespace dreamdb
{

/**
 * @brief 函数调用表达式
 */
class AstFunctionCallExpressionNode : public AstExpressionNode
{
public:
    AstFunctionCallExpressionNode(std::size_t line = 0, std::size_t column = 0);

    AstFunctionCallExpressionNode(const AstFunctionCallExpressionNode &) = delete;

    AstFunctionCallExpressionNode(AstFunctionCallExpressionNode &&) noexcept = default;

    AstFunctionCallExpressionNode & operator=(const AstFunctionCallExpressionNode &) = delete;

    AstFunctionCallExpressionNode & operator=(AstFunctionCallExpressionNode &&) noexcept = default;

    ~AstFunctionCallExpressionNode() override = default;

public:
    /**
     * @brief 设置函数名
     * @param function_name 函数名
     */
    void set_function_name(const std::string & function_name);

    /**
     * @brief 添加参数
     * @param arg 参数表达式
     */
    void add_argument(std::unique_ptr<AstExpressionNode> arg);

    /**
     * @brief 获取函数名
     * @return 函数名
     */
    const std::string & get_function_name() const;

    /**
     * @brief 获取参数列表
     * @return 参数列表
     */
    const std::vector<std::unique_ptr<AstExpressionNode>> & get_arguments() const noexcept;

    /**
     * @brief 是否存在函数名
     * @return 是否存在函数名
     */
    bool has_function_name() const noexcept;

    /**
     * @brief 是否存在参数列表
     * @return 是否存在参数列表
     */
    bool has_arguments() const noexcept;

private:
    std::optional<std::string> function_name_;                   // 函数名
    std::vector<std::unique_ptr<AstExpressionNode>> arguments_;  // 参数列表
};

} // namespace dreamdb
