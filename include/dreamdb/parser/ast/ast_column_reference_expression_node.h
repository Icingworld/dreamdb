#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <optional>

#include "dreamdb/parser/ast/ast_expression_node.h"

namespace dreamdb
{

/**
 * @brief 列引用表达式
 */
class AstColumnReferenceExpressionNode : public AstExpressionNode
{
public:
    AstColumnReferenceExpressionNode(std::size_t line, std::size_t column);

    AstColumnReferenceExpressionNode(const AstColumnReferenceExpressionNode & other) = delete;

    AstColumnReferenceExpressionNode(AstColumnReferenceExpressionNode && other) noexcept = default;

    AstColumnReferenceExpressionNode & operator=(const AstColumnReferenceExpressionNode & other) = delete;

    AstColumnReferenceExpressionNode & operator=(AstColumnReferenceExpressionNode && other) noexcept = default;

    ~AstColumnReferenceExpressionNode() noexcept = default;

public:
    /**
     * @brief 设置数据库名
     * @param database_name 数据库名
     */
    void set_database_name(const std::string & database_name);

    /**
     * @brief 设置集合名
     * @param collection_name 集合名
     */
    void set_collection_name(const std::string & collection_name);

    /**
     * @brief 设置列名
     * @param column_name 列名
     */
    void set_column_name(const std::string & column_name);
    
    /**
     * @brief 获取数据库名
     * @return 数据库名
     */
    const std::string & get_database_name() const;

    /**
     * @brief 获取集合名
     * @return 集合名
     */
    const std::string & get_collection_name() const;

    /**
     * @brief 获取列名
     * @return 列名
     */
    const std::string & get_column_name() const;

    /**
     * @brief 是否设置数据库名
     * @return 是否设置数据库名
     */
    bool has_database_name() const noexcept;

    /**
     * @brief 是否设置集合名
     * @return 是否设置集合名
     */
    bool has_collection_name() const noexcept;

    /**
     * @brief 是否设置列名
     * @return 是否设置列名
     */
    bool has_column_name() const noexcept;

private:
    std::optional<std::string> database_name_;          // 数据库名
    std::optional<std::string> collection_name_;        // 集合名
    std::optional<std::string> column_name_;            // 列名
};

} // namespace dreamdb
