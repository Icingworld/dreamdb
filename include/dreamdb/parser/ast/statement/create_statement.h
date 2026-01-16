#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <variant>
#include <memory>

#include "dreamdb/parser/ast/statement/statement.h"
#include "dreamdb/parser/ast/statement/column_definition.h"

namespace dreamdb::parser::ast
{

class AstExpression;

/**
 * @brief 向量索引 WITH 子句选项
 */
struct AstVIndexWithOption
{
    AstVIndexWithOption(std::string key, std::unique_ptr<AstExpression> value);

    std::string key;                                // 键
    std::unique_ptr<AstExpression> value;           // 值表达式
};

/**
 * @brief CREATE DATABASE 语句操作
 */
struct AstCreateDatabase
{
    std::string database_name;  // 数据库名称
};

/**
 * @brief CREATE COLLECTION 语句操作
 */
struct AstCreateCollection
{
    std::string collection_name;                          // 集合名称
    std::vector<AstColumnDefinition> column_definitions;  // 列定义列表
};

/**
 * @brief CREATE INDEX 语句操作
 */
struct AstCreateIndex
{
    std::string index_name;                    // 索引名称
    std::string collection_name;               // 集合名称
    std::vector<std::string> column_names;     // 列名列表
    std::optional<std::string> index_type;     // 索引类型
};

/**
 * @brief CREATE VINDEX 语句操作
 */
struct AstCreateVIndex
{
    std::string vindex_name;                          // 向量索引名称
    std::string collection_name;                      // 集合名称
    std::string column_name;                          // 列名
    std::optional<std::string> vindex_type;           // 向量索引类型
    std::vector<AstVIndexWithOption> with_clauses;    // WITH 子句列表
};

using AstCreateStatementOperation = std::variant<
    AstCreateDatabase,         // CREATE DATABASE 语句操作
    AstCreateCollection,       // CREATE COLLECTION 语句操作
    AstCreateIndex,            // CREATE INDEX 语句操作
    AstCreateVIndex            // CREATE VINDEX 语句操作
>;

/**
 * @brief CREATE 语句
 */
class AstCreateStatement final : public AstStatement
{
public:
    AstCreateStatement(
        AstCreateStatementOperation operation,
        bool if_not_exists,
        std::size_t line,
        std::size_t column
    );

    ~AstCreateStatement() noexcept override;

public:
    /**
     * @brief 创建 CREATE 语句
     * @param operation CREATE 语句操作
     * @param if_not_exists 是否跳过存在性检查
     * @param line 行号
     * @param column 列号
     * @return CREATE 语句
     */
    static std::unique_ptr<AstCreateStatement> create(
        AstCreateStatementOperation operation,
        bool if_not_exists,
        std::size_t line,
        std::size_t column
    );

public:
    /**
     * @brief 获取 CREATE 语句操作
     * @return CREATE 语句操作
     */
    const AstCreateStatementOperation & operation() const noexcept;

    /**
     * @brief 获取是否跳过存在性检查
     * @return 是否跳过存在性检查
     */
    bool if_not_exists() const noexcept;

    /**
     * @brief 接受语句访问者
     * @param visitor 语句访问者
     */
    void accept(AstStatementVisitor & visitor) const override;

private:
    AstCreateStatementOperation operation_;  // CREATE 语句操作
    bool if_not_exists_;                     // 是否跳过存在性检查
};

} // namespace dreamdb::parser::ast

