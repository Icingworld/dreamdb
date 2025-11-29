#pragma once

#include <cstdint>
#include <variant>
#include <string>

#include "dreamdb/parser/ast/ast_node.h"
#include "dreamdb/common/type.h"

namespace dreamdb
{

/**
 * @brief 字面量类型
 */
enum class LiteralType : std::uint8_t
{
    INTERGER,             // 整数
    FLOAT,                // 浮点数
    STRING,               // 字符串
    BOOLEAN,              // 布尔值
    NULL_VALUE            // 空值
    // TODO: VECTOR, ENUM 等类型
};

/**
 * @brief 不同字面量类型对应的具体值类型
 */
using LiteralValue = std::variant<
    int64_t,            // INTERGER
    double,             // FLOAT
    std::string,        // STRING
    bool,               // BOOLEAN
    NullType            // NULL_VALUE
>;

/**
 * @brief 字面量表达式
 */
class LiteralExpr : public AstNode
{
public:
    LiteralExpr(std::size_t line = 0, std::size_t column = 0);

    LiteralExpr(const LiteralExpr &) = delete;

    LiteralExpr & operator=(const LiteralExpr &) = delete;

    LiteralExpr(LiteralExpr &&) noexcept = default;

    LiteralExpr & operator=(LiteralExpr &&) noexcept = default;

    ~LiteralExpr() override = default;

public:
    /**
     * @brief 设置字面量类型
     * @param type 字面量类型
     */
    void set_literal_type(LiteralType type) noexcept;

    /**
     * @brief 获取字面量类型
     * @return 字面量类型
     */
    LiteralType get_literal_type() const noexcept;

    /**
     * @brief 设置字面量值
     * @param value 字面量值
     */
    void set_value(const LiteralValue & value) noexcept;

    /**
     * @brief 获取字面量值
     * @return 字面量值
     */ 
    const LiteralValue & get_value() const noexcept;
    
    /**
     * @brief 设置是否为空值
     * @param is_null 是否为空值
     */
    void set_null(bool is_null) noexcept;

    /**
     * @brief 获取是否为空值
     * @return 是否为空值
     */
    bool is_null() const noexcept;

public:
    /**
     * @brief 调试输出
     * @return 调试输出
     */
    std::string debug_string() const override;

private:
    LiteralType type;       // 字面量类型
    LiteralValue value;     // 字面量值
};

} // namespace dreamdb
