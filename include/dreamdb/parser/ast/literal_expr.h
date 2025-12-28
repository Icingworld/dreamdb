#pragma once

#include <cstdint>
#include <cstddef>
#include <variant>
#include <string>
#include <vector>

#include "dreamdb/parser/ast/ast_node.h"
#include "dreamdb/common/null.h"

namespace dreamdb
{

/**
 * @brief 字面量表达式
 */
class LiteralExpr : public AstNode
{
public:
    /**
     * @brief 字面量类型
     */
    enum class LiteralType : std::uint8_t
    {
        INTEGER,              // 整数
        FLOAT,                // 浮点数
        STRING,               // 字符串
        BOOLEAN,              // 布尔值
        NULL_VALUE,           // 空值
        VECTOR,               // 向量
    };

    /**
     * @brief 不同字面量类型对应的具体值类型
     */
    using LiteralValue = std::variant<
        std::int64_t,       // INTEGER
        double,             // FLOAT
        std::string,        // STRING
        bool,               // BOOLEAN
        Null,               // NULL_VALUE
        std::vector<float>  // VECTOR
    >;

public:
    LiteralExpr(std::size_t line = 0, std::size_t column = 0);

    LiteralExpr(const LiteralExpr &) = delete;

    LiteralExpr(LiteralExpr &&) noexcept = default;

    LiteralExpr & operator=(const LiteralExpr &) = delete;

    LiteralExpr & operator=(LiteralExpr &&) noexcept = default;

    ~LiteralExpr() override = default;

public:
    /**
     * @brief 设置字面量类型
     * @param literal_type 字面量类型
     */
    void set_literal_type(LiteralType literal_type) noexcept;

    /**
     * @brief 设置字面量值
     * @param literal_value 字面量值
     */
    void set_literal_value(const LiteralValue & literal_value);

    /**
     * @brief 获取字面量类型
     * @return 字面量类型
     */
    LiteralType get_literal_type() const noexcept;

    /**
     * @brief 获取字面量值
     * @return 字面量值
     */ 
    const LiteralValue & get_literal_value() const noexcept;

public:
    /**
     * @brief 调试字符串
     * @return 调试字符串
     */
    std::string debug_string() const override;

private:
    LiteralType literal_type_;      // 字面量类型
    LiteralValue literal_value_;    // 字面量值
};

} // namespace dreamdb
