#pragma once

#include <cstddef>
#include <vector>
#include <string>
#include <memory>

#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb
{

/**
 * @brief 函数调用表达式
 * @details 表示一个函数调用，如 COUNT(*), MAX(column), SUBSTRING(str, 1, 5) 等
 */
class FunctionCallExpr : public AstNode
{
public:
    FunctionCallExpr(std::size_t line = 0, std::size_t column = 0);

    FunctionCallExpr(const FunctionCallExpr &) = delete;

    FunctionCallExpr(FunctionCallExpr &&) noexcept = default;

    FunctionCallExpr & operator=(const FunctionCallExpr &) = delete;

    FunctionCallExpr & operator=(FunctionCallExpr &&) noexcept = default;

    ~FunctionCallExpr() override = default;

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
    void add_argument(std::unique_ptr<AstNode> arg);

    /**
     * @brief 获取函数名
     * @return 函数名
     */
    const std::string & get_function_name() const noexcept;

    /**
     * @brief 获取参数列表
     * @return 参数列表
     */
    const std::vector<std::unique_ptr<AstNode>> & get_arguments() const noexcept;

public:
    /**
     * @brief 调试字符串
     * @return 调试字符串
     */
    std::string debug_string() const override;

private:
    std::string function_name_;                                  // 函数名
    std::vector<std::unique_ptr<AstNode>> arguments_;            // 参数列表
};

} // namespace dreamdb
