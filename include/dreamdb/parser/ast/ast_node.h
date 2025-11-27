#pragma once

#include <cstdint>
#include <string>

namespace dreamdb
{

/**
 * @brief AST 节点类型
 */
enum class AstNodeType : std::uint8_t
{
    // 语句节点
    SELECT_STMT,
    INSERT_STMT,
    DELETE_STMT,
    UPDATE_STMT,
    CREATE_COLLECTION_STMT,
    DROP_COLLECTION_STMT,

    // 表达式节点
    IDENTIFIER_EXPR,
    LITERAL_EXPR,
    BINARY_EXPR,
    UNARY_EXPR,
    FUNCTION_CALL_EXPR
};

/**
 * @brief AST 节点基类
 * @details 每个具体的语句或表达式节点都继承自此类
 */
class AstNode
{
public:
    AstNode(AstNodeType type, std::size_t line = 0, std::size_t column = 0);

    virtual ~AstNode() = default;

public:
    /**
     * @brief 获取节点类型
     * @return 节点类型
     */
    AstNodeType get_type() const noexcept;

    /**
     * @brief 获取行号
     * @return 行号
     */
    std::size_t get_line() const noexcept;

    /**
     * @brief 获取列号
     * @return 列号
     */
    std::size_t get_column() const noexcept;

    /**
     * @brief 调试输出
     * @return 节点的字符串描述
     */
    virtual std::string debug_string() const = 0;

protected:
    /**
     * @brief 设置源位置
     * @param line 行号
     * @param column 列号
     */
    void set_source_location(std::size_t line, std::size_t column) noexcept;

private:
    AstNodeType type;       // 节点类型
    std::size_t line;       // 行号
    std::size_t column;     // 列号
};

} // namespace dreamdb

