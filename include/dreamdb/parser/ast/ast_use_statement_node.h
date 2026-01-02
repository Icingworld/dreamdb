#pragma once

#include <cstddef>
#include <string>
#include <optional>

#include "dreamdb/parser/ast/ast_statement_node.h"

namespace dreamdb
{

/**
 * @brief USE 语句节点
 */
class AstUseStatementNode : public AstStatementNode
{
public:
    AstUseStatementNode(std::size_t line = 0, std::size_t column = 0);

    AstUseStatementNode(const AstUseStatementNode &) = delete;

    AstUseStatementNode(AstUseStatementNode &&) noexcept = default;

    AstUseStatementNode & operator=(const AstUseStatementNode &) = delete;

    AstUseStatementNode & operator=(AstUseStatementNode &&) noexcept = default;

    ~AstUseStatementNode() noexcept override = default;

public:
    /**
     * @brief 设置数据库名称
     * @param database_name 数据库名称
     */
    void set_database_name(const std::string & database_name);

    /**
     * @brief 获取数据库名称
     * @return 数据库名称
     */
    const std::string & get_database_name() const noexcept;

    /**
     * @brief 是否存在数据库名称
     * @return 是否存在数据库名称
     */
    bool has_database_name() const noexcept;

private:
    std::optional<std::string> database_name_;  // 数据库名称
};

} // namespace dreamdb
