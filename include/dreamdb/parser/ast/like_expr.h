#pragma once

#include <cstddef>
#include <memory>

#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb
{

/**
 * @brief LIKE 表达式
 * @details 表示一个 LIKE 表达式，如 column LIKE 'pattern' 或 column NOT LIKE 'pattern'
 */
class LikeExpr : public AstNode
{
public:
    LikeExpr(std::size_t line = 0, std::size_t column = 0);

    LikeExpr(const LikeExpr & other) = delete;

    LikeExpr(LikeExpr && other) noexcept = default;

    LikeExpr & operator=(const LikeExpr & other) = delete;

    LikeExpr & operator=(LikeExpr && other) noexcept = default;

    ~LikeExpr() noexcept = default;

public:
    /**
     * @brief 设置左侧表达式
     * @param left 左侧表达式
     */
    void set_left(std::unique_ptr<AstNode> left) noexcept;

    /**
     * @brief 设置是否为 NOT LIKE 表达式
     * @param is_not_like 是否为 NOT LIKE 表达式
     */
    void set_is_not_like(bool is_not_like) noexcept;

    /**
     * @brief 设置右侧表达式（模式字符串）
     * @param right 右侧表达式（模式字符串）
     */
    void set_right(std::unique_ptr<AstNode> right) noexcept;

    /**
     * @brief 获取左侧表达式
     * @return 左侧表达式
     */
    const AstNode * get_left() const noexcept;

    /**
     * @brief 获取是否为 NOT LIKE 表达式
     * @return 是否为 NOT LIKE 表达式
     */
    bool is_not_like() const noexcept;

    /**
     * @brief 获取右侧表达式（模式字符串）
     * @return 右侧表达式
     */
    const AstNode * get_right() const noexcept;

public:
    /**
     * @brief 调试字符串
     * @return 调试字符串
     */
    std::string debug_string() const override;

private:
    std::unique_ptr<AstNode> left_;                    // 左侧表达式，一般用于存储列名
    bool is_not_like_;                                 // 是否为 NOT LIKE 表达式
    std::unique_ptr<AstNode> right_;                   // 右侧表达式（模式字符串）
};

} // namespace dreamdb
