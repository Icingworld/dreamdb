#pragma once

#include <cstddef>
#include <string>
#include <memory>
#include <optional>

#include "dreamdb/parser/ast/ast_node.h"
#include "dreamdb/common/type.h"

namespace dreamdb
{

/**
 * @brief DELETE 语句节点
 * @details 表示 DELETE FROM <collection_name> [WHERE <where_clause>] [ORDER BY <order_column> {ASC | DESC}] [LIMIT <limit_clause>] 语句
 */
class DeleteStmt : public AstNode
{
public:
    DeleteStmt(std::size_t line = 0, std::size_t column = 0);

    DeleteStmt(const DeleteStmt &) = delete;

    DeleteStmt(DeleteStmt &&) noexcept = default;

    DeleteStmt & operator=(const DeleteStmt &) = delete;

    DeleteStmt & operator=(DeleteStmt &&) noexcept = default;

    ~DeleteStmt() override = default;

public:
    /**
     * @brief 设置集合名
     * @param collection_name 集合名
     */
    void set_collection_name(const std::string & collection_name);

    /**
     * @brief 设置 where 条件子句
     * @param where where 条件子句
     */
    void set_where_clause(std::unique_ptr<AstNode> where_clause) noexcept;

    /**
     * @brief 设置排序列名
     * @param order_column 排序列名
     */
    void set_order_column(const std::string & order_column) noexcept;

    /**
     * @brief 设置排序类型
     * @param order_type 排序类型
     */
    void set_order_type(Direction order_type) noexcept;

    /**
     * @brief 设置限制数量
     * @param limit 限制数量
     */
    void set_limit(std::size_t limit) noexcept;

    /**
     * @brief 获取集合名
     * @return 集合名
     */
    const std::string & get_collection_name() const noexcept;

    /**
     * @brief 获取 where 条件子句
     * @return where 条件子句
     */
    const AstNode * get_where_clause() const noexcept;

    /**
     * @brief 获取排序列名
     * @return 排序列名
     */
    const std::optional<std::string> & get_order_column() const noexcept;

    /**
     * @brief 获取排序类型
     * @return 排序类型
     */
    std::optional<Direction> get_order_type() const noexcept;

    /**
     * @brief 获取限制数量
     * @return 限制数量
     */
    std::optional<std::size_t> get_limit() const noexcept;

public:
    /**
     * @brief 调试字符串
     * @return 调试字符串
     */
    std::string debug_string() const override;

private:
    std::string collection_name_;                // 集合名
    std::unique_ptr<AstNode> where_clause_;      // where 条件子句
    std::optional<std::string> order_column_;    // 排序列名
    std::optional<Direction> order_type_;        // 排序类型
    std::optional<std::size_t> limit_;           // 限制数量
};

} // namespace dreamdb
