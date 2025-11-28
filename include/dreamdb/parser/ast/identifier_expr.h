#pragma once

#include <cstdint>
#include <vector>
#include <string>

#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb
{
    
enum class IdentifierType : std::uint8_t
{
    COLLECTION,        // 集合名
    COLUMN,            // 列名
    ALIAS,             // 别名
    FUNCTION           // 函数名
};

/**
 * @brief 标识符表达式
 */
class IdentifierExpr : public AstNode
{
public:
    IdentifierExpr(std::size_t line, std::size_t column);

    IdentifierExpr(const IdentifierExpr & other) = delete;

    IdentifierExpr & operator=(const IdentifierExpr & other) = delete;

    IdentifierExpr(IdentifierExpr && other) noexcept = default;

    IdentifierExpr & operator=(IdentifierExpr && other) noexcept = default;

    ~IdentifierExpr() noexcept = default;

public:
    /**
     * @brief 设置标识符类型
     * @param type 标识符类型
     */
    void set_type(IdentifierType type) noexcept;

    /**
     * @brief 获取标识符类型
     * @return 标识符类型
     */
    IdentifierType get_type() const noexcept;

    /**
     * @brief 设置标识符部分
     * @param parts 标识符部分
     */
    void set_parts(const std::vector<std::string> & parts) noexcept;

    /**
     * @brief 添加标识符部分
     * @param part 标识符部分
     */
    void add_part(const std::string & part) noexcept;

    /**
     * @brief 获取标识符部分
     * @return 标识符部分
     */
    const std::vector<std::string> & get_parts() const noexcept;

    /**
     * @brief 设置原始文本
     * @param original_text 原始文本
     */
    void set_original_text(const std::string & original_text) noexcept;

    /**
     * @brief 获取原始文本
     * @return 原始文本
     */
    const std::string & get_original_text() const noexcept;

public:
    /**
     * @brief 调试输出
     * @return 调试输出
     */
    std::string debug_string() const override;

private:
    IdentifierType type;                    // 标识符类型
    std::vector<std::string> parts;         // 标识符部分
    std::string original_text;              // 原始文本
};

} // namespace dreamdb
