#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <memory>

#include "dreamdb/parser/ast/ast_expression_node.h"

namespace dreamdb
{

/**
 * @brief 二元运算符类型
 */
enum class AstBinaryOperatorType : std::uint8_t
{
    AST_BINARY_OPERATOR_UNKNOWN,    // 未知运算符

    // 算术运算符
    AST_BINARY_OPERATOR_PLUS,           // +
    AST_BINARY_OPERATOR_MINUS,          // -
    AST_BINARY_OPERATOR_MULTIPLY,       // *
    AST_BINARY_OPERATOR_DIVIDE,         // /
    AST_BINARY_OPERATOR_MODULO,         // %

    // 比较运算符
    AST_BINARY_OPERATOR_EQUAL,          // =
    AST_BINARY_OPERATOR_NOT_EQUAL,      // !=
    AST_BINARY_OPERATOR_LESS_THAN,      // <
    AST_BINARY_OPERATOR_GREATER_THAN,   // >
    AST_BINARY_OPERATOR_LESS_EQUAL,     // <=
    AST_BINARY_OPERATOR_GREATER_EQUAL,  // >=

    // 逻辑运算符
    AST_BINARY_OPERATOR_AND,            // AND
    AST_BINARY_OPERATOR_OR,             // OR
};

/**
 * @brief 二元表达式
 */
class AstBinaryExpressionNode : public AstExpressionNode
{
public:
    AstBinaryExpressionNode(std::size_t line = 0, std::size_t column = 0);

    AstBinaryExpressionNode(const AstBinaryExpressionNode &) = delete;

    AstBinaryExpressionNode(AstBinaryExpressionNode &&) noexcept = default;

    AstBinaryExpressionNode & operator=(const AstBinaryExpressionNode &) = delete;

    AstBinaryExpressionNode & operator=(AstBinaryExpressionNode &&) noexcept = default;

    ~AstBinaryExpressionNode() override = default;

public:
    /**
     * @brief 设置二元运算符类型
     * @param operator_type 运算符类型
     */
    void set_operator_type(AstBinaryOperatorType operator_type) noexcept;

    /**
     * @brief 设置左操作数
     * @param left 左操作数表达式
     */
    void set_left(std::unique_ptr<AstExpressionNode> left) noexcept;

    /**
     * @brief 设置右操作数
     * @param right 右操作数表达式
     */
    void set_right(std::unique_ptr<AstExpressionNode> right) noexcept;

    /**
     * @brief 获取二元运算符类型
     * @return 运算符类型
     */
    AstBinaryOperatorType get_operator_type() const noexcept;

    /**
     * @brief 获取左操作数
     * @return 左操作数表达式指针
     */
    const AstExpressionNode & get_left() const;

    /**
     * @brief 获取右操作数
     * @return 右操作数表达式指针
     */
    const AstExpressionNode & get_right() const;

    /**
     * @brief 是否存在二元运算符类型
     * @return 是否存在二元运算符类型
     */
    bool has_operator_type() const noexcept;

    /**
     * @brief 是否存在左操作数
     * @return 是否存在左操作数
     */
    bool has_left() const noexcept;

    /**
     * @brief 是否存在右操作数
     * @return 是否存在右操作数
     */
    bool has_right() const noexcept;

private:
    AstBinaryOperatorType operator_type_;            // 二元运算符类型
    std::unique_ptr<AstExpressionNode> left_;        // 左操作数表达式
    std::unique_ptr<AstExpressionNode> right_;       // 右操作数表达式
};

} // namespace dreamdb
