#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <memory>
#include <optional>

#include "dreamdb/parser/ast/ast_statement_node.h"

namespace dreamdb
{

class AstExpressionNode;

/**
 * @brief INSERT 语句节点
 */
class AstInsertStatementNode : public AstStatementNode
{
public:
    AstInsertStatementNode(std::size_t line = 0, std::size_t column = 0);

    AstInsertStatementNode(const AstInsertStatementNode &) = delete;

    AstInsertStatementNode(AstInsertStatementNode &&) noexcept = default;

    AstInsertStatementNode & operator=(const AstInsertStatementNode &) = delete;

    AstInsertStatementNode & operator=(AstInsertStatementNode &&) noexcept = default;

    ~AstInsertStatementNode() noexcept override;

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
    void add_value(std::unique_ptr<AstExpressionNode> value);

    /**
     * @brief 获取集合名
     * @return 集合名
     */
    const std::string & get_collection_name() const noexcept;

    /**
     * @brief 获取列名列表
     * @details 如果未添加列名，则返回空列表，表示按照表结构顺序插入
     * @return 列名列表
     */
    const std::vector<std::string> & get_column_names() const noexcept;

    /**
     * @brief 获取值列表
     * @return 值列表
     */
    const std::vector<std::unique_ptr<AstExpressionNode>> & get_values() const noexcept;

    /**
     * @brief 是否存在集合名
     * @return 是否存在集合名
     */
    bool has_collection_name() const noexcept;

    /**
     * @brief 是否存在列名列表
     * @return 是否存在列名列表
     */
    bool has_column_names() const noexcept;

    /**
     * @brief 是否存在值列表
     * @return 是否存在值列表
     */
    bool has_values() const noexcept;

private:
    std::optional<std::string> collection_name_;                  // 集合名
    std::vector<std::string> column_names_;                       // 列名列表
    std::vector<std::unique_ptr<AstExpressionNode>> values_;      // 值列表
};

} // namespace dreamdb
