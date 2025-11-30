#pragma once

#include <cstdint>
#include <string>
#include <memory>

#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb
{

/**
 * @brief 二元运算符类型
 */
enum class BinaryOperatorType : std::uint8_t
{
    // 算术运算符
    PLUS,           // +
    MINUS,          // -
    MULTIPLY,       // *
    DIVIDE,         // /
    MODULO,         // %
    
    // 比较运算符
    EQUAL,          // =
    NOT_EQUAL,      // != 或 <>
    LESS_THAN,      // <
    GREATER_THAN,   // >
    LESS_EQUAL,     // <=
    GREATER_EQUAL,  // >=
    
    // 逻辑运算符
    AND,            // AND
    OR              // OR
};

/**
 * @brief 二元表达式
 * @details 表示一个二元运算符作用于两个操作数，如 a + b, x = y, condition AND other 等
 */
class BinaryExpr : public AstNode
{
public:
    BinaryExpr(std::size_t line = 0, std::size_t column = 0);

    BinaryExpr(const BinaryExpr &) = delete;

    BinaryExpr & operator=(const BinaryExpr &) = delete;

    BinaryExpr(BinaryExpr &&) noexcept = default;

    BinaryExpr & operator=(BinaryExpr &&) noexcept = default;

    ~BinaryExpr() override = default;

public:
    /**
     * @brief 设置二元运算符类型
     * @param op_type 运算符类型
     */
    void set_op_type(BinaryOperatorType op_type) noexcept;

    /**
     * @brief 获取二元运算符类型
     * @return 运算符类型
     */
    BinaryOperatorType get_op_type() const noexcept;

    /**
     * @brief 设置左操作数
     * @param left 左操作数表达式
     */
    void set_left(std::unique_ptr<AstNode> left);

    /**
     * @brief 获取左操作数（只读）
     * @return 左操作数表达式指针，可能为 nullptr
     */
    const AstNode * get_left() const noexcept;

    /**
     * @brief 获取左操作数（可修改）
     * @return 左操作数表达式指针，可能为 nullptr
     */
    AstNode * get_left() noexcept;

    /**
     * @brief 设置右操作数
     * @param right 右操作数表达式
     */
    void set_right(std::unique_ptr<AstNode> right);

    /**
     * @brief 获取右操作数（只读）
     * @return 右操作数表达式指针，可能为 nullptr
     */
    const AstNode * get_right() const noexcept;

    /**
     * @brief 获取右操作数（可修改）
     * @return 右操作数表达式指针，可能为 nullptr
     */
    AstNode * get_right() noexcept;

public:
    /**
     * @brief 调试输出
     * @return 调试输出
     */
    std::string debug_string() const override;

private:
    BinaryOperatorType op_type;              // 二元运算符类型
    std::unique_ptr<AstNode> left;           // 左操作数表达式
    std::unique_ptr<AstNode> right;          // 右操作数表达式
};

} // namespace dreamdb
