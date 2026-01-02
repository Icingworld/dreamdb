#pragma once

#include <cstddef>
#include <string>
#include <optional>

#include "dreamdb/parser/ast/ast_statement_node.h"

namespace dreamdb
{

/**
 * @brief DESCRIBE 语句节点
 */
class AstDescribeStatementNode : public AstStatementNode
{
public:
    AstDescribeStatementNode(std::size_t line = 0, std::size_t column = 0);

    AstDescribeStatementNode(const AstDescribeStatementNode &) = delete;

    AstDescribeStatementNode(AstDescribeStatementNode &&) noexcept = default;

    AstDescribeStatementNode & operator=(const AstDescribeStatementNode &) = delete;

    AstDescribeStatementNode & operator=(AstDescribeStatementNode &&) noexcept = default;

    ~AstDescribeStatementNode() noexcept override = default;

public:
    /**
     * @brief 设置集合名称
     * @param collection_name 集合名称
     */
    void set_collection_name(const std::string & collection_name);

    /**
     * @brief 获取集合名称
     * @return 集合名称
     */
    const std::string & get_collection_name() const noexcept;

    /**
     * @brief 是否存在集合名称
     * @return 是否存在集合名称
     */
    bool has_collection_name() const noexcept;

private:
    std::optional<std::string> collection_name_;   // 集合名称
};

} // namespace dreamdb
