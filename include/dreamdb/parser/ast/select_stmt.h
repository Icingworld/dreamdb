#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb
{

/**
 * @brief SELECT 语句的 SELECT 列表条目
 */
class SelectItem
{
public:
    /**
     * @brief 类型
     */
    enum class Type : std::uint8_t
    {
        STAR,           // *
        EXPRESSION      // 表达式或字段
    };

public:
    SelectItem();

    SelectItem(const SelectItem &) = delete;

    SelectItem & operator=(const SelectItem &) = delete;

    SelectItem(SelectItem &&) noexcept = default;

    SelectItem & operator=(SelectItem &&) noexcept = default;

    ~SelectItem() = default;

public:
    /**
     * @brief 创建一个星号条目
     * @return 星号条目
     */
    static SelectItem create_star();

    /**
     * @brief 创建一个表达式条目
     * @param expr 表达式
     * @param alias 别名
     * @return 表达式条目
     */
    static SelectItem create_expression(std::unique_ptr<AstNode> expr, const std::string & alias = "");

public:
    Type type;                              // 类型
    std::unique_ptr<AstNode> expression;    // STAR 时为空
    std::string alias;                      // 可选别名
};

/**
 * @brief SELECT 语句节点
 */
class SelectStmt : public AstNode
{
public:
    SelectStmt(std::size_t line = 0, std::size_t column = 0);

    SelectStmt(const SelectStmt &) = delete;

    SelectStmt & operator=(const SelectStmt &) = delete;

    SelectStmt(SelectStmt &&) noexcept = default;

    SelectStmt & operator=(SelectStmt &&) noexcept = default;

    ~SelectStmt() override = default;

public:
    /**
     * @brief 设置表名
     * @param table 表名
     */
    void set_table_name(const std::string & table);

    /**
     * @brief 获取表名
     * @return 表名
     */
    const std::string & get_table_name() const noexcept;

    /**
     * @brief 添加一个 SELECT 列表条目
     * @param item SELECT 列表条目
     */
    void add_select_item(SelectItem item);

     /**
      * @brief 获取 SELECT 列表
      * @return SELECT 列表
     */
    const std::vector<SelectItem> & get_select_items() const noexcept;

    /**
     * @brief 设置 WHERE 子句
     * @param expr WHERE 子句
     */
    void set_where_clause(std::unique_ptr<AstNode> expr);

    /**
     * @brief 获取 WHERE 子句
     * @return WHERE 子句
     */
    const AstNode * get_where_clause() const noexcept;

    /**
     * @brief 设置 LIMIT 子句
     * @param limit LIMIT 子句
     */
    void set_limit(std::size_t limit);

    /**
     * @brief 获取 LIMIT 子句
     * @return LIMIT 子句
     */
    std::optional<std::size_t> get_limit() const noexcept;

    /**
     * @brief 调试字符串
     * @return 调试字符串
     */
    std::string debug_string() const override;

private:
    std::string table_name;                     // 表名
    std::vector<SelectItem> select_items;       // SELECT 列表
    std::unique_ptr<AstNode> where_clause;      // WHERE 子句
    std::optional<std::size_t> limit;           // LIMIT 子句
};

} // namespace dreamdb
