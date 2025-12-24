#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>

#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb
{

/**
 * @brief 标识符表达式
 */
class IdentifierExpr : public AstNode
{
public:
    /**
     * @brief 标识符类型
     */
    enum class IdentifierType : std::uint8_t
    {
        COLLECTION,        // 集合名
        COLUMN,            // 列名
        ALIAS,             // 别名
        FUNCTION           // 函数名
    };

public:
    IdentifierExpr(std::size_t line, std::size_t column);

    IdentifierExpr(const IdentifierExpr & other) = delete;

    IdentifierExpr(IdentifierExpr && other) noexcept = default;

    IdentifierExpr & operator=(const IdentifierExpr & other) = delete;

    IdentifierExpr & operator=(IdentifierExpr && other) noexcept = default;

    ~IdentifierExpr() noexcept = default;

public:
    /**
     * @brief 设置标识符类型
     * @param type 标识符类型
     */
    void set_identifier_type(IdentifierType type) noexcept;

    /**
     * @brief 设置标识符部分
     * @param parts 标识符部分
     */
    void set_parts(const std::vector<std::string> & parts);

    /**
     * @brief 添加标识符部分
     * @param part 标识符部分
     */
    void add_part(const std::string & part);
    
    /**
     * @brief 设置原始标识符
     * @param original_identifier 原始标识符
     */
    void set_original_identifier(const std::string & original_identifier);

    /**
     * @brief 获取标识符类型
     * @return 标识符类型
     */
    IdentifierType get_identifier_type() const noexcept;

    /**
     * @brief 获取标识符部分
     * @return 标识符部分
     */
    const std::vector<std::string> & get_parts() const noexcept;

    /**
     * @brief 获取原始标识符
     * @return 原始标识符
     */
    const std::string & get_original_identifier() const;

public:
    /**
     * @brief 调试字符串
     * @return 调试字符串
     */
    std::string debug_string() const override;

private:
    IdentifierType type_;                    // 标识符类型
    std::vector<std::string> parts_;         // 标识符部分
    std::string original_identifier_;        // 原始标识符
};

} // namespace dreamdb
