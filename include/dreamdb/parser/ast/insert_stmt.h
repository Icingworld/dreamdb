#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <memory>

#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb
{

/**
 * @brief INSERT 语句节点
 * @details INSERT INTO <collection_name> (<column_name1>, <column_name2>, ...) VALUES (<value1>, <value2>, ...);
 */
class InsertStmt : public AstNode
{
public:
    InsertStmt(std::size_t line = 0, std::size_t column = 0);

    InsertStmt(const InsertStmt &) = delete;

    InsertStmt(InsertStmt &&) noexcept = default;

    InsertStmt & operator=(const InsertStmt &) = delete;

    InsertStmt & operator=(InsertStmt &&) noexcept = default;

    ~InsertStmt() override = default;

public:
    /**
     * @brief 设置集合名
     * @param collection_name 集合名
     */
    void set_collection_name(const std::string & collection_name);

    /**
     * @brief 添加列名
     * @param column 列名
     */
    void add_column_name(const std::string & column);

    /**
     * @brief 添加值
     * @param value 值节点
     */
    void add_value(std::unique_ptr<AstNode> value);

    /**
     * @brief 获取列名列表
     * @details 如果未添加列名，则返回空列表，表示按照表结构顺序插入
     * @return 列名列表
     */
    const std::vector<std::string> & get_column_names() const noexcept;

    /**
     * @brief 获取集合名
     * @return 集合名
     */
    const std::string & get_collection_name() const noexcept;

    /**
     * @brief 获取值列表
     * @return 值列表
     */
    const std::vector<std::unique_ptr<AstNode>> & get_values() const noexcept;

public:
    /**
     * @brief 调试字符串
     * @return 调试字符串
     */
    std::string debug_string() const override;

private:
    std::string collection_name_;                       // 集合名
    std::vector<std::string> column_names_;             // 列名列表
    std::vector<std::unique_ptr<AstNode>> values_;      // 值列表
};

} // namespace dreamdb
