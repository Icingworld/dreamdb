#pragma once

#include <cstddef>
#include <memory>

#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb
{

/**
 * @brief BETWEEN 表达式
 * @details 表示一个 BETWEEN 表达式，如 column BETWEEN 1 AND 10
 */
class BetweenExpr : public AstNode
{
public:
    BetweenExpr(std::size_t line = 0, std::size_t column = 0);

    BetweenExpr(const BetweenExpr & other) = delete;

    BetweenExpr(BetweenExpr && other) noexcept = default;

    BetweenExpr & operator=(const BetweenExpr & other) = delete;

    BetweenExpr & operator=(BetweenExpr && other) noexcept = default;

    ~BetweenExpr() noexcept = default;

public:
    /**
     * @brief 设置左侧表达式
     * @param left 左侧表达式
     */
    void set_left(std::unique_ptr<AstNode> left) noexcept;

    /**
     * @brief 设置是否为 NOT BETWEEN 表达式
     * @param is_not_between 是否为 NOT BETWEEN 表达式
     */
    void set_is_not_between(bool is_not_between) noexcept;

    /**
     * @brief 设置起始值（最小值）
     * @param min_value 起始值表达式
     */
    void set_min_value(std::unique_ptr<AstNode> min_value) noexcept;

    /**
     * @brief 设置结束值（最大值）
     * @param max_value 结束值表达式
     */
    void set_max_value(std::unique_ptr<AstNode> max_value) noexcept;

    /**
     * @brief 获取左侧表达式
     * @return 左侧表达式
     */
    const AstNode * get_left() const noexcept;

    /**
     * @brief 获取是否为 NOT BETWEEN 表达式
     * @return 是否为 NOT BETWEEN 表达式
     */
    bool is_not_between() const noexcept;

    /**
     * @brief 获取起始值（最小值）
     * @return 起始值表达式
     */
    const AstNode * get_min_value() const noexcept;

    /**
     * @brief 获取结束值（最大值）
     * @return 结束值表达式
     */
    const AstNode * get_max_value() const noexcept;

public:
    /**
     * @brief 调试字符串
     * @return 调试字符串
     */
    std::string debug_string() const override;

private:
    std::unique_ptr<AstNode> left_;                    // 左侧表达式，一般用于存储列名
    bool is_not_between_;                              // 是否为 NOT BETWEEN 表达式
    std::unique_ptr<AstNode> min_value_;               // 起始值（最小值）表达式
    std::unique_ptr<AstNode> max_value_;               // 结束值（最大值）表达式
};

} // namespace dreamdb

