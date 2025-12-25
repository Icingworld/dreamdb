#pragma once

#include <cstddef>
#include <memory>

#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb
{

/**
 * @brief NULL 表达式
 * @details 表示一个 IS NULL 或 IS NOT NULL 表达式，如 column IS NULL 或 column IS NOT NULL
 */
class NullExpr : public AstNode
{
public:
    NullExpr(std::size_t line = 0, std::size_t column = 0);

    NullExpr(const NullExpr & other) = delete;

    NullExpr(NullExpr && other) noexcept = default;

    NullExpr & operator=(const NullExpr & other) = delete;

    NullExpr & operator=(NullExpr && other) noexcept = default;

    ~NullExpr() noexcept = default;

public:
    /**
     * @brief 设置左侧表达式
     * @param left 左侧表达式
     */
    void set_left(std::unique_ptr<AstNode> left) noexcept;

    /**
     * @brief 设置是否为 IS NOT NULL 表达式
     * @param is_not_null 是否为 IS NOT NULL 表达式
     */
    void set_is_not_null(bool is_not_null) noexcept;

    /**
     * @brief 获取左侧表达式
     * @return 左侧表达式
     */
    const AstNode * get_left() const noexcept;

    /**
     * @brief 获取是否为 IS NOT NULL 表达式
     * @return 是否为 IS NOT NULL 表达式
     */
    bool is_not_null() const noexcept;

public:
    /**
     * @brief 调试字符串
     * @return 调试字符串
     */
    std::string debug_string() const override;

private:
    std::unique_ptr<AstNode> left_;                    // 左侧表达式，一般用于存储列名
    bool is_not_null_;                                 // 是否为 IS NOT NULL 表达式
};

} // namespace dreamdb
