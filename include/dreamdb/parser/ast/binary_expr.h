#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <memory>

#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb
{

/**
 * @brief 二元表达式
 * @details 表示一个二元运算符作用于两个操作数，如 a + b, x = y, condition AND other 等
 */
class BinaryExpr : public AstNode
{
public:
    /**
     * @brief 二元运算符类型
     */
    enum class OperatorType : std::uint8_t
    {
        // 算术运算符
        DB_PLUS,           // +
        DB_MINUS,          // -
        DB_MULTIPLY,       // *
        DB_DIVIDE,         // /
        DB_MODULO,         // %

        // 比较运算符
        DB_EQUAL,          // =
        DB_NOT_EQUAL,      // !=
        DB_LESS_THAN,      // <
        DB_GREATER_THAN,   // >
        DB_LESS_EQUAL,     // <=
        DB_GREATER_EQUAL,  // >=

        // 逻辑运算符
        DB_AND,            // AND
        DB_OR,             // OR
    };

public:
    BinaryExpr(std::size_t line = 0, std::size_t column = 0);

    BinaryExpr(const BinaryExpr &) = delete;

    BinaryExpr(BinaryExpr &&) noexcept = default;

    BinaryExpr & operator=(const BinaryExpr &) = delete;

    BinaryExpr & operator=(BinaryExpr &&) noexcept = default;

    ~BinaryExpr() override = default;

public:
    /**
     * @brief 设置二元运算符类型
     * @param operator_type 运算符类型
     */
    void set_operator_type(OperatorType operator_type) noexcept;

    /**
     * @brief 设置左操作数
     * @param left 左操作数表达式
     */
    void set_left(std::unique_ptr<AstNode> left) noexcept;

    /**
     * @brief 设置右操作数
     * @param right 右操作数表达式
     */
    void set_right(std::unique_ptr<AstNode> right) noexcept;

    /**
     * @brief 获取二元运算符类型
     * @return 运算符类型
     */
    OperatorType get_operator_type() const noexcept;

    /**
     * @brief 获取左操作数
     * @return 左操作数表达式指针
     */
    const AstNode * get_left() const noexcept;

    /**
     * @brief 获取右操作数
     * @return 右操作数表达式指针
     */
    const AstNode * get_right() const noexcept;

public:
    /**
     * @brief 调试字符串
     * @return 调试字符串
     */
    std::string debug_string() const override;

private:
    OperatorType operator_type_;            // 二元运算符类型
    std::unique_ptr<AstNode> left_;         // 左操作数表达式
    std::unique_ptr<AstNode> right_;        // 右操作数表达式
};

} // namespace dreamdb
