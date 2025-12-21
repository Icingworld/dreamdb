#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <memory>
#include <utility>

#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb
{

/**
 * @brief UPDATE 语句节点
 * @details 表示 UPDATE <collection_name> SET <column_name1> = <value1>, ... WHERE <where_clause> 语句
 */
class UpdateStmt : public AstNode
{
public:
    UpdateStmt(std::size_t line = 0, std::size_t column = 0);

    UpdateStmt(const UpdateStmt &) = delete;

    UpdateStmt(UpdateStmt &&) noexcept = default;

    UpdateStmt & operator=(const UpdateStmt &) = delete;

    UpdateStmt & operator=(UpdateStmt &&) noexcept = default;

    ~UpdateStmt() override = default;

public:
    /**
     * @brief 设置集合名
     * @param collection_name 集合名
     */
    void set_collection_name(const std::string & collection_name);

    /**
     * @brief 添加一个赋值项
     * @param column_name 列名
     * @param value 值表达式
     */
    void add_assignment(const std::string & column_name, std::unique_ptr<AstNode> value);

    /**
     * @brief 设置 WHERE 子句
     * @param where_clause WHERE 条件
     */
    void set_where_clause(std::unique_ptr<AstNode> where_clause);

    /**
     * @brief 获取集合名
     * @return 集合名
     */
    const std::string & get_collection_name() const noexcept;

    /**
     * @brief 获取所有赋值
     * @return 赋值列表，每个元素是 (列名, 值表达式) 的 pair
     */
    const std::vector<std::pair<std::string, std::unique_ptr<AstNode>>> & get_assignments() const noexcept;

    /**
     * @brief 获取 WHERE 子句
     * @return WHERE 条件表达式，如果没有返回 nullptr
     */
    const AstNode * get_where_clause() const noexcept;

public:
    /**
     * @brief 调试字符串
     * @return 调试字符串
     */
    std::string debug_string() const override;

private:
    std::string collection_name_;                                                // 集合名
    std::vector<std::pair<std::string, std::unique_ptr<AstNode>>> assignments_;  // SET 子句：列名 -> 值表达式
    std::unique_ptr<AstNode> where_clause_;                                      // WHERE 条件
};

} // namespace dreamdb
