#pragma once

#include <string>
#include <memory>

#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb
{

/**
 * @brief DELETE 语句节点
 */
class DeleteStmt : public AstNode
{
public:
    DeleteStmt(std::size_t line = 0, std::size_t column = 0);

    DeleteStmt(const DeleteStmt &) = delete;

    DeleteStmt & operator=(const DeleteStmt &) = delete;

    DeleteStmt(DeleteStmt &&) noexcept = default;

    DeleteStmt & operator=(DeleteStmt &&) noexcept = default;

    ~DeleteStmt() override = default;

public:
    /**
     * @brief 设置表名
     * @param table 表名
     */
    void set_collection_name(const std::string & collection_name);

    /**
     * @brief 获取表名
     * @return 表名
     */
    const std::string & get_collection_name() const;

    /**
     * @brief 设置 where 条件子句
     * @param where where 条件子句
     */
    void set_where_clause(std::unique_ptr<AstNode> where);

    /**
     * @brief 获取 where 条件子句
     * @return where 条件子句
     */
    const AstNode * get_where_clause() const;

    /**
     * @brief 是否存在 where 条件子句
     * @return 是否存在 where 条件子句
     */
    bool has_where_clause() const;

public:
    /**
     * @brief 调试字符串
     * @return 调试字符串
     */
    std::string debug_string() const override;

private:
    std::string collection_name;                // 集合名
    std::unique_ptr<AstNode> where_clause;      // where 条件子句
};

} // namespace dreamdb
