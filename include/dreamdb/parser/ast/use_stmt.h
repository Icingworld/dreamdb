#pragma once

#include <cstddef>
#include <string>

#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb
{

/**
 * @brief USE 语句节点
 * @details 表示 USE database_name 语句，用于切换当前数据库
 */
class UseStmt : public AstNode
{
public:
    UseStmt(std::size_t line = 0, std::size_t column = 0);

    UseStmt(const UseStmt &) = delete;

    UseStmt(UseStmt &&) noexcept = default;

    UseStmt & operator=(const UseStmt &) = delete;

    UseStmt & operator=(UseStmt &&) noexcept = default;

    ~UseStmt() override = default;

public:
    /**
     * @brief 设置数据库名称
     * @param name 数据库名称
     */
    void set_database_name(const std::string & name);

    /**
     * @brief 获取数据库名称
     * @return 数据库名称
     */
    const std::string & get_database_name() const noexcept;

public:
    /**
     * @brief 调试字符串
     * @return 调试字符串
     */
    std::string debug_string() const override;

private:
    std::string database_name_;  // 数据库名称
};

} // namespace dreamdb
