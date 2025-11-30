#pragma once

#include <cstdint>
#include <string>
#include <memory>

#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb
{

/**
 * @brief 一元运算符类型
 */
enum class UnaryOperatorType : std::uint8_t
{
    NOT,        // 逻辑非
    MINUS,      // 负号
    PLUS        // 正号
};

class UnaryExpr : public AstNode
{
public:
    UnaryExpr(std::size_t line = 0, std::size_t column = 0);

    UnaryExpr(const UnaryExpr &) = delete;

    UnaryExpr & operator=(const UnaryExpr &) = delete;

    UnaryExpr(UnaryExpr &&) noexcept = default;

    UnaryExpr & operator=(UnaryExpr &&) noexcept = default;

    ~UnaryExpr() override = default;

public:
    /**
     * @brief 设置一元运算符类型
     * @param op_type 运算符类型
     */
    void set_op_type(UnaryOperatorType op_type) noexcept;

    /**
     * @brief 获取一元运算符类型
     * @return 运算符类型
     */
    UnaryOperatorType get_op_type() const noexcept;

    /**
     * @brief 设置操作数
     * @param operand 操作数表达式
     */
    void set_operand(std::unique_ptr<AstNode> operand);

    /**
     * @brief 获取操作数（只读）
     * @return 操作数表达式指针，可能为 nullptr
     */
    const AstNode * get_operand() const noexcept;

    /**
     * @brief 获取操作数（可修改）
     * @return 操作数表达式指针，可能为 nullptr
     */
    AstNode * get_operand() noexcept;

public:
    /**
     * @brief 调试输出
     * @return 调试输出
     */
    std::string debug_string() const override;

private:
    UnaryOperatorType op_type;                  // 一元运算符类型
    std::unique_ptr<AstNode> operand;           // 操作数表达式
};

} // namespace dreamdb
