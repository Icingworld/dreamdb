#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <memory>
#include <optional>

#include "dreamdb/parser/ast/ast_node.h"
#include "dreamdb/common/type.h"

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
    enum class SelectItemType : std::uint8_t
    {
        STAR,           // *
        EXPRESSION      // 表达式或字段
    };

public:
    SelectItem();

    SelectItem(const SelectItem &) = delete;

    SelectItem(SelectItem &&) noexcept = default;

    SelectItem & operator=(const SelectItem &) = delete;

    SelectItem & operator=(SelectItem &&) noexcept = default;

    ~SelectItem() = default;

public:
    /**
     * @brief 创建一个星号条目
     * @return 星号条目
     */
    static SelectItem create_star_item();

    /**
     * @brief 创建一个表达式条目
     * @param expression 表达式
     * @param alias 别名
     * @return 表达式条目
     */
    static SelectItem create_expression_item(std::unique_ptr<AstNode> expression, const std::string & alias = "");

public:
    /**
     * @brief 获取类型
     * @return 类型
     */
    SelectItemType get_select_item_type() const noexcept;

    /**
     * @brief 获取表达式
     * @return 表达式
     */
    const AstNode * get_select_item_expression() const noexcept;
    
    /**
     * @brief 获取别名
     * @return 别名
     */
    const std::string & get_select_item_alias() const noexcept;

private:
    SelectItemType select_item_type_;                              // 类型
    std::unique_ptr<AstNode> select_item_expression_;              // STAR 时为空
    std::string select_item_alias_;                                // 可选别名
};

/**
 * @brief ORDER BY 子句的排序项
 */
class OrderByItem
{
public:
    OrderByItem();

    OrderByItem(const OrderByItem &) = delete;

    OrderByItem(OrderByItem &&) noexcept = default;

    OrderByItem & operator=(const OrderByItem &) = delete;

    OrderByItem & operator=(OrderByItem &&) noexcept = default;

    ~OrderByItem() = default;

public:
    /**
     * @brief 设置排序表达式
     * @param expression 排序表达式（可以是列名、函数调用等）
     */
    void set_expression(std::unique_ptr<AstNode> expression) noexcept;

    /**
     * @brief 设置排序类型
     * @param order_type 排序类型
     */
    void set_order_type(Direction order_type) noexcept;

    /**
     * @brief 获取排序表达式
     * @return 排序表达式
     */
    const AstNode * get_expression() const noexcept;

    /**
     * @brief 获取排序类型
     * @return 排序类型
     */
    Direction get_order_type() const noexcept;

private:
    std::unique_ptr<AstNode> expression_;  // 排序表达式
    Direction order_type_;                 // 排序类型
};

/**
 * @brief SELECT 语句节点
 * @details 表示 SELECT <column_list> FROM <collection_name> [WHERE <condition>] [GROUP BY <column> [, <column> ...]] [HAVING <condition>] [ORDER BY <column> [ASC | DESC] [, <column> [ASC | DESC] ...]] [LIMIT <limit>] 语句
 */
class SelectStmt : public AstNode
{
public:
    SelectStmt(std::size_t line = 0, std::size_t column = 0);

    SelectStmt(const SelectStmt &) = delete;

    SelectStmt(SelectStmt &&) noexcept = default;

    SelectStmt & operator=(const SelectStmt &) = delete;

    SelectStmt & operator=(SelectStmt &&) noexcept = default;

    ~SelectStmt() override = default;

public:
    /**
     * @brief 设置集合名
     * @param collection_name 集合名
     */
    void set_collection_name(const std::string & collection_name);

    /**
     * @brief 添加一个 SELECT 列表条目
     * @param item SELECT 列表条目
     */
    void add_select_item(SelectItem && item);

    /**
     * @brief 设置 WHERE 子句
     * @param expr WHERE 子句
     */
    void set_where_clause(std::unique_ptr<AstNode> expr) noexcept;

    /**
     * @brief 添加一个 GROUP BY 表达式
     * @param expression GROUP BY 表达式
     */
    void add_group_by_expression(std::unique_ptr<AstNode> expression) noexcept;

    /**
     * @brief 设置 HAVING 子句
     * @param expr HAVING 子句
     */
    void set_having_clause(std::unique_ptr<AstNode> expr) noexcept;

    /**
     * @brief 添加一个 ORDER BY 项
     * @param item ORDER BY 项
     */
    void add_order_by_item(OrderByItem && item) noexcept;

    /**
     * @brief 设置 LIMIT 子句
     * @param limit LIMIT 子句
     */
    void set_limit(std::size_t limit) noexcept;

    /**
     * @brief 获取集合名
     * @return 集合名
     */
    const std::string & get_collection_name() const noexcept;

    /**
     * @brief 获取 SELECT 列表
     * @return SELECT 列表
     */
    const std::vector<SelectItem> & get_select_items() const noexcept;

    /**
     * @brief 获取 WHERE 子句
     * @return WHERE 子句
     */
    const AstNode * get_where_clause() const noexcept;

    /**
     * @brief 获取 GROUP BY 表达式列表
     * @return GROUP BY 表达式列表
     */
    const std::vector<std::unique_ptr<AstNode>> & get_group_by_expressions() const noexcept;

    /**
     * @brief 获取 HAVING 子句
     * @return HAVING 子句
     */
    const AstNode * get_having_clause() const noexcept;

    /**
     * @brief 获取 ORDER BY 项列表
     * @return ORDER BY 项列表
     */
    const std::vector<OrderByItem> & get_order_by_items() const noexcept;

    /**
     * @brief 获取 LIMIT 子句
     * @return LIMIT 子句
     */
    std::optional<std::size_t> get_limit() const noexcept;

public:
    /**
     * @brief 调试字符串
     * @return 调试字符串
     */
    std::string debug_string() const override;

private:
    std::string collection_name_;                          // 集合名
    std::vector<SelectItem> select_items_;                 // SELECT 列表
    std::unique_ptr<AstNode> where_clause_;                // WHERE 子句
    std::vector<std::unique_ptr<AstNode>> group_by_exprs_; // GROUP BY 表达式列表
    std::unique_ptr<AstNode> having_clause_;               // HAVING 子句
    std::vector<OrderByItem> order_by_items_;              // ORDER BY 项列表
    std::optional<std::size_t> limit_;                     // LIMIT 子句
};

} // namespace dreamdb
