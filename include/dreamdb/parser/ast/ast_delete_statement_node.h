#pragma once

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
 * @brief DELETE 语句节点
 */
class AstDeleteStatementNode : public AstStatementNode
{
public:
    AstDeleteStatementNode(std::size_t line = 0, std::size_t column = 0);

    AstDeleteStatementNode(const AstDeleteStatementNode &) = delete;

    AstDeleteStatementNode(AstDeleteStatementNode &&) noexcept = default;

    AstDeleteStatementNode & operator=(const AstDeleteStatementNode &) = delete;

    AstDeleteStatementNode & operator=(AstDeleteStatementNode &&) noexcept = default;

    ~AstDeleteStatementNode() noexcept override;

public:
    /**
     * @brief 设置集合名
     * @param collection_name 集合名
     */
    void set_collection_name(const std::string & collection_name);

    /**
     * @brief 设置 WHERE 子句
     * @param where_clause WHERE 子句
     */
    void set_where_clause(std::unique_ptr<AstExpressionNode> where_clause) noexcept;

    /**
     * @brief 添加一个 ORDER BY 项
     * @param item ORDER BY 项
     */
    void add_order_by_item(OrderByItem && item) noexcept;

    /**
     * @brief 设置限制数量
     * @param limit 限制数量
     */
    void set_limit(std::size_t limit) noexcept;

    /**
     * @brief 设置偏移量
     * @param offset 偏移量
     */
    void set_offset(std::size_t offset) noexcept;

    /**
     * @brief 获取集合名
     * @return 集合名
     */
    const std::string & get_collection_name() const noexcept;

    /**
     * @brief 获取 WHERE 子句
     * @return WHERE 子句
     */
    const AstExpressionNode & get_where_clause() const noexcept;

    /**
     * @brief 获取 ORDER BY 子句
     * @return ORDER BY 子句
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
     * @brief 是否存在 WHERE 子句
     * @return 是否存在 WHERE 子句
     */
    bool has_where_clause() const noexcept;

    /**
     * @brief 是否存在 ORDER BY 子句
     * @return 是否存在 ORDER BY 子句
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
    std::optional<std::string> collection_name_;           // 集合名
    std::unique_ptr<AstExpressionNode> where_clause_;      // WHERE 子句
    std::vector<OrderByItem> order_by_items_;              // ORDER BY 子句
    std::optional<std::size_t> limit_;                     // LIMIT 子句
    std::optional<std::size_t> offset_;                    // OFFSET 子句
};

} // namespace dreamdb
