#pragma once

#include <cstdint>
#include <cstddef>
#include <memory>
#include <string>
#include <variant>

#include "dreamdb/parser/ast/expression/expression.h"
#include "dreamdb/common/null.h"

namespace dreamdb::parser::ast
{

/**
 * @brief 字面量值
 */
using AstLiteralValue = std::variant<
    std::int64_t,              // 整数
    double,                    // 浮点数
    std::string,               // 字符串
    bool,                      // 布尔值
    Null                       // 空值
>;

/**
 * @brief 字面量表达式
 */
class AstLiteralExpression final : public AstExpression
{
private:
    AstLiteralExpression(AstLiteralValue value, std::size_t line, std::size_t column);

public:
    ~AstLiteralExpression() noexcept override = default;

public:
    /**
     * @brief 创建整数字面量表达式
     * @param value 整数值
     * @param line 行号
     * @param column 列号
     * @return 整数字面量表达式
     */
    static std::unique_ptr<AstLiteralExpression> create_integer(std::int64_t value, std::size_t line, std::size_t column);

    /**
     * @brief 创建浮点字面量表达式
     * @param value 浮点数值
     * @param line 行号
     * @param column 列号
     * @return 浮点字面量表达式
     */
    static std::unique_ptr<AstLiteralExpression> create_float(double value, std::size_t line, std::size_t column);

    /**
     * @brief 创建字符串字面量表达式
     * @param value 字符串值
     * @param line 行号
     * @param column 列号
     * @return 字符串字面量表达式
     */
    static std::unique_ptr<AstLiteralExpression> create_string(std::string value, std::size_t line, std::size_t column);

    /**
     * @brief 创建布尔字面量表达式
     * @param value 布尔值
     * @param line 行号
     * @param column 列号
     * @return 布尔字面量表达式
     */
    static std::unique_ptr<AstLiteralExpression> create_boolean(bool value, std::size_t line, std::size_t column);

    /**
     * @brief 创建空字面量表达式
     * @param line 行号
     * @param column 列号
     * @return 空字面量表达式
     */
    static std::unique_ptr<AstLiteralExpression> create_null(std::size_t line, std::size_t column);

public:
    /**
     * @brief 获取字面量值
     * @return 字面量值
     */
    const AstLiteralValue & value() const noexcept;

    /**
     * @brief 接受表达式访问者
     * @param visitor 表达式访问者
     */
    void accept(AstExpressionVisitor & visitor) const override;

private:
    AstLiteralValue value_;           // 字面量值
};

} // namespace dreamdb::parser::ast
