#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <memory>

#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb
{

class UnaryExpr : public AstNode
{
public:
    /**
     * @brief 一元运算符类型
     */
    enum class OperatorType : std::uint8_t
    {
        DB_NOT,        // 逻辑非
        DB_MINUS,      // 负号
        DB_PLUS        // 正号
    };

public:
    UnaryExpr(std::size_t line = 0, std::size_t column = 0);

    UnaryExpr(const UnaryExpr &) = delete;

    UnaryExpr(UnaryExpr &&) noexcept = default;

    UnaryExpr & operator=(const UnaryExpr &) = delete;

    UnaryExpr & operator=(UnaryExpr &&) noexcept = default;

    ~UnaryExpr() override = default;

public:
    /**
     * @brief 设置一元运算符类型
     * @param operator_type 运算符类型
     */
    void set_operator_type(OperatorType operator_type) noexcept;

    /**
     * @brief 设置操作数
     * @param operand 操作数表达式
     */
    void set_operand(std::unique_ptr<AstNode> operand) noexcept;

    /**
     * @brief 获取一元运算符类型
     * @return 运算符类型
     */
    OperatorType get_operator_type() const noexcept;

    /**
     * @brief 获取操作数
     * @return 操作数表达式指针
     */
    const AstNode * get_operand() const noexcept;

public:
    /**
     * @brief 调试字符串
     * @return 调试字符串
     */
    std::string debug_string() const override;

private:
    OperatorType type_;                     // 一元运算符类型
    std::unique_ptr<AstNode> operand_;      // 操作数表达式
};

} // namespace dreamdb
