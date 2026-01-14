#pragma once

#include <cstddef>
#include <string>
#include <optional>
#include <memory>

#include "dreamdb/parser/ast/expression/expression.h"

namespace dreamdb::parser::ast
{

/**
 * @brief 列引用表达式
 */
class AstColumnReferenceExpression : public AstExpression
{
private:
    AstColumnReferenceExpression(
        std::optional<std::string> database_name,
        std::optional<std::string> collection_name,
        std::string column_name,
        std::size_t line,
        std::size_t column
    );

public:
    ~AstColumnReferenceExpression() noexcept override = default;

public:
    /**
     * @brief 创建包含数据库名和集合名的列引用表达式
     * @param database_name 数据库名
     * @param collection_name 集合名
     * @param column_name 列名
     * @param line 行号
     * @param column 列号
     * @return 包含数据库名和集合名的列引用表达式
     */
    static std::unique_ptr<AstColumnReferenceExpression> create(
        std::string database_name,
        std::string collection_name,
        std::string column_name,
        std::size_t line,
        std::size_t column
    );

    /**
     * @brief 创建包含集合名的列引用表达式
     * @param collection_name 集合名
     * @param column_name 列名
     * @param line 行号
     * @param column 列号
     * @return 包含集合名的列引用表达式
     */
    static std::unique_ptr<AstColumnReferenceExpression> create(
        std::string collection_name,
        std::string column_name,
        std::size_t line,
        std::size_t column
    );

    /**
     * @brief 创建包含列名的列引用表达式
     * @param column_name 列名
     * @param line 行号
     * @param column 列号
     * @return 包含列名的列引用表达式
     */
    static std::unique_ptr<AstColumnReferenceExpression> create(
        std::string column_name,
        std::size_t line,
        std::size_t column
    );

public:    
    /**
     * @brief 获取数据库名
     * @return 数据库名
     */
    const std::optional<std::string> & database_name() const noexcept;

    /**
     * @brief 获取集合名
     * @return 集合名
     */
    const std::optional<std::string> & collection_name() const noexcept;

    /**
     * @brief 获取列名
     * @return 列名
     */
    const std::string & column_name() const noexcept;

    /**
     * @brief 接受表达式访问者
     * @param visitor 表达式访问者
     */
    void accept(AstExpressionVisitor & visitor) const override;

private:
    std::optional<std::string> database_name_;      // 数据库名
    std::optional<std::string> collection_name_;    // 集合名
    std::string column_name_;                       // 列名
};

} // namespace dreamdb::parser::ast
