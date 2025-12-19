#pragma once

#include <cstddef>
#include <string>
#include <memory>

#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb
{

/**
 * @brief DELETE 语句节点
 * @details 表示 DELETE FROM <collection_name> [WHERE <where_clause>] 语句
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
     * @brief 获取集合名
     * @return 集合名
     */
    const std::string & get_collection_name() const noexcept;

    /**
     * @brief 设置 where 条件子句
     * @param where where 条件子句
     */
    void set_where_clause(std::unique_ptr<AstNode> where_clause) noexcept;

    /**
     * @brief 获取 where 条件子句
     * @return where 条件子句
     */
    const AstNode * get_where_clause() const noexcept;

    /**
     * @brief 是否存在 where 条件子句
     * @return 是否存在 where 条件子句
     */
    bool has_where_clause() const noexcept;

public:
    /**
     * @brief 调试字符串
     * @return 调试字符串
     */
    std::string debug_string() const override;

private:
    std::string collection_name_;                // 集合名
    std::unique_ptr<AstNode> where_clause_;      // where 条件子句
};

} // namespace dreamdb
