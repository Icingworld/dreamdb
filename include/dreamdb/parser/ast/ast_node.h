#pragma once

#include <cstddef>

namespace dreamdb::parser::ast
{

/**
 * @brief Ast 语法树节点基类
 * @details 每个具体的语句或表达式节点都继承自此类
 */
class AstNode
{
protected:
    AstNode(std::size_t line = 0, std::size_t column = 0) noexcept;

public:
    AstNode(const AstNode &) = delete;

    AstNode(AstNode &&) noexcept = default;

    AstNode & operator=(const AstNode &) = delete;

    AstNode & operator=(AstNode &&) noexcept = default;

    virtual ~AstNode() noexcept = default;

public:
    /**
     * @brief 获取行号
     * @return 行号
     */
    std::size_t line() const noexcept;

    /**
     * @brief 获取列号
     * @return 列号
     */
    std::size_t column() const noexcept;

private:
    std::size_t line_;       // 行号
    std::size_t column_;     // 列号
};

} // namespace dreamdb::parser::ast
