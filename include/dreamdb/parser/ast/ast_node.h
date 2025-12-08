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
    SELECT_STMT,                // 查询语句
    INSERT_STMT,                // 插入语句
    DELETE_STMT,                // 删除语句
    UPDATE_STMT,                // 更新语句
    CREATE_STMT,                // 创建语句
    DROP_STMT,                  // 删除语句
    USE_STMT,                   // 切换数据库语句

    // 表达式节点
    LITERAL_EXPR,               // 字面量表达式，如整数、浮点数、字符串等
    IDENTIFIER_EXPR,            // 标识符表达式，如字段名、表名、别名等
    UNARY_EXPR,                 // 一元表达式，如负号、取反等
    BINARY_EXPR,                // 二元表达式，如加减乘除、比较等
    FUNCTION_CALL_EXPR          // 函数调用表达式，如数学函数、字符串函数等
};

/**
 * @brief AST 节点基类
 * @details 每个具体的语句或表达式节点都继承自此类
 */
class AstNode
{
public:
    AstNode(AstNodeType type, std::size_t line = 0, std::size_t column = 0);

    AstNode(const AstNode &) = delete;

    AstNode & operator=(const AstNode &) = delete;

    AstNode(AstNode &&) noexcept = default;

    AstNode & operator=(AstNode &&) noexcept = default;

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

public:
    /**
     * @brief 调试输出
     * @return 调试输出
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

