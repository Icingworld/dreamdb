#pragma once

#include <cstddef>
#include <vector>
#include <memory>

#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb
{

/**
 * @brief IN 表达式
 * @details 表示一个 IN 表达式，如 IN (1, 2, 3)
 */
class InExpr : public AstNode
{
public:
    InExpr(std::size_t line = 0, std::size_t column = 0);

    InExpr(const InExpr & other) = delete;

    InExpr(InExpr && other) noexcept = default;

    InExpr & operator=(const InExpr & other) = delete;

    InExpr & operator=(InExpr && other) noexcept = default;

    ~InExpr() noexcept = default;

public:
    /**
     * @brief 设置左侧表达式
     * @param left 左侧表达式
     */
    void set_left(std::unique_ptr<AstNode> left) noexcept;

    /**
     * @brief 设置是否为 NOT IN 表达式
     * @param is_not_in 是否为 NOT IN 表达式
     */
    void set_is_not_in(bool is_not_in) noexcept;

    /**
     * @brief 添加 IN 表达式的值
     * @param value IN 表达式的值
     */
    void add_value(std::unique_ptr<AstNode> value) noexcept;

    /**
     * @brief 获取左侧表达式
     * @return 左侧表达式
     */
    const AstNode * get_left() const noexcept;

    /**
     * @brief 获取是否为 NOT IN 表达式
     * @return 是否为 NOT IN 表达式
     */
    bool is_not_in() const noexcept;

    /**
     * @brief 获取 IN 表达式的值列表
     * @return IN 表达式的值列表
     */
    const std::vector<std::unique_ptr<AstNode>> & get_values() const noexcept;

public:
    /**
     * @brief 调试字符串
     * @return 调试字符串
     */
    std::string debug_string() const override;

private:
    std::unique_ptr<AstNode> left_;                     // 左侧表达式，一般用于储存列名
    bool is_not_in_;                                    // 是否为 NOT IN 表达式
    std::vector<std::unique_ptr<AstNode>> values_;      // IN 表达式的值列表
};

} // namespace dreamdb
