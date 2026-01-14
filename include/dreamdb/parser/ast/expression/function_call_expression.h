#pragma once

#include <cstddef>
#include <vector>
#include <string>
#include <memory>
#include <optional>

#include "dreamdb/parser/ast/expression/expression.h"

namespace dreamdb::parser::ast
{

/**
 * @brief 函数调用表达式
 */
class AstFunctionCallExpression : public AstExpression
{
public:
    AstFunctionCallExpression(
        std::string function_name,
        std::vector<std::unique_ptr<AstExpression>> arguments,
        std::size_t line,
        std::size_t column
    );

    ~AstFunctionCallExpression() noexcept override = default;

public:
    /**
     * @brief 创建函数调用表达式
     * @param function_name 函数名
     * @param arguments 参数列表
     * @param line 行号
     * @param column 列号
     * @return 函数调用表达式
     */
    static std::unique_ptr<AstFunctionCallExpression> create(
        std::string function_name,
        std::vector<std::unique_ptr<AstExpression>> arguments,
        std::size_t line,
        std::size_t column
    );

public:
    /**
     * @brief 获取函数名
     * @return 函数名
     */
    const std::string & function_name() const noexcept;

    /**
     * @brief 获取指定索引的参数
     * @param index 参数索引
     * @return 参数表达式
     */
    const AstExpression & argument_at(std::size_t index) const noexcept;

    /**
     * @brief 获取参数数量
     * @return 参数数量
     */
    std::size_t argument_count() const noexcept;

    /**
     * @brief 接受表达式访问者
     * @param visitor 表达式访问者
     */
    void accept(AstExpressionVisitor & visitor) const override;

private:
    std::string function_name_;                              // 函数名
    std::vector<std::unique_ptr<AstExpression>> arguments_;  // 参数列表
};

} // namespace dreamdb::parser::ast
