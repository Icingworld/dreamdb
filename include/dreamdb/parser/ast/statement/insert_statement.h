#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <memory>

#include "dreamdb/parser/ast/statement/statement.h"

namespace dreamdb::parser::ast
{

class AstExpression;

/**
 * @brief INSERT 语句
 */
class AstInsertStatement : public AstStatement
{
public:
    AstInsertStatement(
        std::string collection_name,
        std::vector<std::string> column_names,
        std::vector<std::unique_ptr<AstExpression>> values,
        std::size_t line,
        std::size_t column
    );

    ~AstInsertStatement() noexcept override = default;

public:
    /**
     * @brief 创建 INSERT 语句
     * @param collection_name 集合名
     * @param column_names 列名列表（可选，如果为空则按表结构顺序插入）
     * @param values 值表达式列表
     * @param line 行号
     * @param column 列号
     * @return INSERT 语句
     */
    static std::unique_ptr<AstInsertStatement> create(
        std::string collection_name,
        std::vector<std::string> column_names,
        std::vector<std::unique_ptr<AstExpression>> values,
        std::size_t line,
        std::size_t column
    );

public:
    /**
     * @brief 获取集合名
     * @return 集合名
     */
    const std::string & collection_name() const noexcept;

    /**
     * @brief 是否存在列名列表
     * @return 是否存在列名列表
     * @details 如果未指定列名，则表示按照表结构顺序插入
     */
    bool has_column_names() const noexcept;

    /**
     * @brief 获取指定索引的列名
     * @param index 索引
     * @return 列名
     */
    const std::string & column_name_at(std::size_t index) const noexcept;

    /**
     * @brief 获取列名数量
     * @return 列名数量
     */
    std::size_t column_name_count() const noexcept;

    /**
     * @brief 获取指定索引的值表达式
     * @param index 索引
     * @return 值表达式
     */
    const AstExpression & value_at(std::size_t index) const noexcept;

    /**
     * @brief 获取值表达式数量
     * @return 值表达式数量
     */
    std::size_t value_count() const noexcept;

    /**
     * @brief 接受语句访问者
     * @param visitor 语句访问者
     */
    void accept(AstStatementVisitor & visitor) const override;

private:
    std::string collection_name_;                                   // 集合名
    std::vector<std::string> column_names_;                         // 列名列表（可选）
    std::vector<std::unique_ptr<AstExpression>> values_;            // 值表达式列表
};

} // namespace dreamdb::parser::ast
