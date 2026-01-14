#pragma once

#include <cstddef>
#include <string>
#include <variant>
#include <memory>

#include "dreamdb/parser/ast/statement/statement.h"

namespace dreamdb::parser::ast
{

/**
 * @brief DROP DATABASE 语句操作
 */
struct AstDropDatabase
{
    std::string database_name;  // 数据库名称
};

/**
 * @brief DROP COLLECTION 语句操作
 */
struct AstDropCollection
{
    std::string collection_name;  // 集合名称
};

/**
 * @brief DROP INDEX 语句操作
 */
struct AstDropIndex
{
    std::string index_name;       // 索引名称
    std::string collection_name;  // 集合名称
};

/**
 * @brief DROP VINDEX 语句操作
 */
struct AstDropVIndex
{
    std::string vindex_name;      // 向量索引名称
    std::string collection_name;  // 集合名称
};

using AstDropStatementOperation = std::variant<
    AstDropDatabase,         // DROP DATABASE 语句操作
    AstDropCollection,       // DROP COLLECTION 语句操作
    AstDropIndex,            // DROP INDEX 语句操作
    AstDropVIndex            // DROP VINDEX 语句操作
>;

/**
 * @brief DROP 语句
 */
class AstDropStatement : public AstStatement
{
public:
    AstDropStatement(
        AstDropStatementOperation operation,
        bool if_exists,
        std::size_t line,
        std::size_t column
    );

    ~AstDropStatement() noexcept override = default;

public:
    /**
     * @brief 创建 DROP 语句
     * @param operation DROP 语句操作
     * @param if_exists 是否存在性检查
     * @param line 行号
     * @param column 列号
     * @return DROP 语句
     */
    static std::unique_ptr<AstDropStatement> create(
        AstDropStatementOperation operation,
        bool if_exists,
        std::size_t line,
        std::size_t column
    );

public:
    /**
     * @brief 获取 DROP 语句操作
     * @return DROP 语句操作
     */
    const AstDropStatementOperation & operation() const noexcept;

    /**
     * @brief 获取是否存在性检查
     * @return 是否存在性检查
     */
    bool if_exists() const noexcept;

    /**
     * @brief 接受语句访问者
     * @param visitor 语句访问者
     */
    void accept(AstStatementVisitor & visitor) const override;

private:
    AstDropStatementOperation operation_;  // DROP 语句操作
    bool if_exists_;                       // 是否存在性检查
};

} // namespace dreamdb::parser::ast
