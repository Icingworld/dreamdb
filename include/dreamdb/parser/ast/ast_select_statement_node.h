#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <memory>
#include <optional>

#include "dreamdb/parser/ast/ast_statement_node.h"
#include "dreamdb/parser/ast/order_by_item.h"

namespace dreamdb
{

class AstExpressionNode;

/**
 * @brief SELECT 列表条目类型
 */
enum class SelectItemType : std::uint8_t
{
    SELECT_ITEM_STAR,           // *
    SELECT_ITEM_EXPRESSION,     // 表达式或字段
};

/**
 * @brief SELECT 语句的 SELECT 列表条目
 */
class SelectItem
{
private:
    SelectItem();

public:
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
    static SelectItem create_expression_item(std::unique_ptr<AstExpressionNode> expression, const std::string & alias = "");

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
    const AstExpressionNode & get_select_item_expression() const noexcept;

    /**
     * @brief 获取别名
     * @return 别名
     */
    const std::string & get_select_item_alias() const noexcept;

    /**
     * @brief 是否存在表达式
     * @return 是否存在表达式
     */
    bool has_select_item_expression() const noexcept;

    /**
     * @brief 是否存在别名
     * @return 是否存在别名
     */
    bool has_select_item_alias() const noexcept;

private:
    SelectItemType select_item_type_;                              // 类型
    std::unique_ptr<AstExpressionNode> select_item_expression_;    // 列表表达式
    std::optional<std::string> select_item_alias_;                 // 可选别名
};

/**
 * @brief SELECT 语句节点
 */
class AstSelectStatementNode : public AstStatementNode
{
public:
    AstSelectStatementNode(std::size_t line = 0, std::size_t column = 0);

    AstSelectStatementNode(const AstSelectStatementNode &) = delete;

    AstSelectStatementNode(AstSelectStatementNode &&) noexcept = default;

    AstSelectStatementNode & operator=(const AstSelectStatementNode &) = delete;

    AstSelectStatementNode & operator=(AstSelectStatementNode &&) noexcept = default;

    ~AstSelectStatementNode() noexcept override;

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
     * @param where_clause WHERE 子句
     */
    void set_where_clause(std::unique_ptr<AstExpressionNode> where_clause) noexcept;

    /**
     * @brief 添加一个 GROUP BY 子句
     * @param group_by_clause GROUP BY 子句
     */
    void add_group_by_clause(std::unique_ptr<AstExpressionNode> group_by_clause) noexcept;

    /**
     * @brief 设置 HAVING 子句
     * @param having_clause HAVING 子句
     */
    void set_having_clause(std::unique_ptr<AstExpressionNode> having_clause) noexcept;

    /**
     * @brief 添加一个 ORDER BY 项
     * @param order_by_item ORDER BY 项
     */
    void add_order_by_item(OrderByItem && order_by_item) noexcept;

    /**
     * @brief 设置 LIMIT 子句
     * @param limit LIMIT 子句
     */
    void set_limit(std::size_t limit) noexcept;

    /**
     * @brief 设置 OFFSET 子句
     * @param offset OFFSET 子句
     */
    void set_offset(std::size_t offset) noexcept;

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
    const AstExpressionNode & get_where_clause() const noexcept;

    /**
     * @brief 获取 GROUP BY 子句列表
     * @return GROUP BY 子句列表
     */
    const std::vector<std::unique_ptr<AstExpressionNode>> & get_group_by_clauses() const noexcept;

    /**
     * @brief 获取 HAVING 子句
     * @return HAVING 子句
     */
    const AstExpressionNode & get_having_clause() const noexcept;

    /**
     * @brief 获取 ORDER BY 项列表
     * @return ORDER BY 项列表
     */
    const std::vector<OrderByItem> & get_order_by_items() const noexcept;

    /**
     * @brief 获取 LIMIT 子句
     * @return LIMIT 子句
     */
    std::size_t get_limit() const noexcept;

    /**
     * @brief 获取 OFFSET 子句
     * @return OFFSET 子句
     */
    std::size_t get_offset() const noexcept;

    /**
     * @brief 是否存在集合名
     * @return 是否存在集合名
     */
    bool has_collection_name() const noexcept;

    /**
     * @brief 是否存在 SELECT 列表
     * @return 是否存在 SELECT 列表
     */
    bool has_select_items() const noexcept;

    /**
     * @brief 是否存在 WHERE 子句
     * @return 是否存在 WHERE 子句
     */
    bool has_where_clause() const noexcept;

    /**
     * @brief 是否存在 GROUP BY 子句列表
     * @return 是否存在 GROUP BY 子句列表
     */
    bool has_group_by_clauses() const noexcept;

    /**
     * @brief 是否存在 HAVING 子句
     * @return 是否存在 HAVING 子句
     */
    bool has_having_clause() const noexcept;

    /**
     * @brief 是否存在 ORDER BY 项列表
     * @return 是否存在 ORDER BY 项列表
     */
    bool has_order_by_items() const noexcept;

    /**
     * @brief 是否存在 LIMIT 子句
     * @return 是否存在 LIMIT 子句
     */
    bool has_limit() const noexcept;

    /**
     * @brief 是否存在 OFFSET 子句
     * @return 是否存在 OFFSET 子句
     */
    bool has_offset() const noexcept;

private:
    std::optional<std::string> collection_name_;                           // 集合名
    std::vector<SelectItem> select_items_;                                 // SELECT 列表
    std::unique_ptr<AstExpressionNode> where_clause_;                      // WHERE 子句
    std::vector<std::unique_ptr<AstExpressionNode>> group_by_clauses_;     // GROUP BY 子句列表
    std::unique_ptr<AstExpressionNode> having_clause_;                     // HAVING 子句
    std::vector<OrderByItem> order_by_items_;                              // ORDER BY 项列表
    std::optional<std::size_t> limit_;                                     // LIMIT 子句
    std::optional<std::size_t> offset_;                                    // OFFSET 子句
};

} // namespace dreamdb
