#pragma once

#include <string>
#include <vector>
#include <memory>
#include <utility>

#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb
{

/**
 * @brief UPDATE 语句节点
 * @details 表示 UPDATE ... SET ... WHERE ... 语句
 */
class UpdateStmt : public AstNode
{
public:
    UpdateStmt(std::size_t line = 0, std::size_t column = 0);

    UpdateStmt(const UpdateStmt &) = delete;

    UpdateStmt & operator=(const UpdateStmt &) = delete;

    UpdateStmt(UpdateStmt &&) noexcept = default;

    UpdateStmt & operator=(UpdateStmt &&) noexcept = default;

    ~UpdateStmt() override = default;

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
     * @brief 添加一个赋值
     * @param column 列名
     * @param value 值表达式
     */
    void add_assignment(const std::string & column, std::unique_ptr<AstNode> value);

    /**
     * @brief 获取所有赋值
     * @return 赋值列表，每个元素是 (列名, 值表达式) 的 pair
     */
    const std::vector<std::pair<std::string, std::unique_ptr<AstNode>>> & get_assignments() const noexcept;

    /**
     * @brief 获取赋值数量
     * @return 赋值数量
     */
    std::size_t get_assignment_count() const noexcept;

    /**
     * @brief 设置 WHERE 子句
     * @param expr WHERE 条件表达式
     */
    void set_where_clause(std::unique_ptr<AstNode> expr);

    /**
     * @brief 获取 WHERE 子句
     * @return WHERE 条件表达式，如果没有返回 nullptr
     */
    const AstNode * get_where_clause() const noexcept;

    /**
     * @brief 检查是否有 WHERE 条件
     * @return 如果有 WHERE 条件返回 true
     */
    bool has_where_clause() const noexcept;

public:
    /**
     * @brief 调试字符串
     * @return 调试字符串
     */
    std::string debug_string() const override;

private:
    std::string table_name;                                                     // 表名
    std::vector<std::pair<std::string, std::unique_ptr<AstNode>>> assignments;  // SET 子句：列名 -> 值表达式
    std::unique_ptr<AstNode> where_clause;                                      // WHERE 条件
};

} // namespace dreamdb
