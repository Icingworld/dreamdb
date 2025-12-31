#pragma once

#include <string>
#include <vector>
#include <memory>

#include "dreamdb/expression/expression.h"

namespace dreamdb
{

/**
 * @brief 函数表达式
 */
class FunctionExpression : public Expression
{
public:
    explicit FunctionExpression(const std::string & function_name, std::vector<std::unique_ptr<Expression>> arguments);

    FunctionExpression(const FunctionExpression &) noexcept = delete;

    FunctionExpression(FunctionExpression &&) noexcept = default;

    FunctionExpression & operator=(const FunctionExpression &) noexcept = delete;

    FunctionExpression & operator=(FunctionExpression &&) noexcept = default;

    ~FunctionExpression() noexcept = default;

public:
    /**
     * @brief 获取函数名称
     * @return 函数名称
     */
    const std::string & get_function_name() const noexcept;

    /**
     * @brief 获取参数列表
     * @return 参数列表
     */
    const std::vector<std::unique_ptr<Expression>> & get_arguments() const noexcept;

    /**
     * @brief 获取可变参数列表
     * @return 可变参数列表
     * @details 该接口供 Optimizer 使用
     */
    std::vector<std::unique_ptr<Expression>> & get_mutable_arguments() noexcept;

private:
    std::string function_name_;                             // 函数名称
    std::vector<std::unique_ptr<Expression>> arguments_;    // 参数列表
};

} // namespace dreamdb
