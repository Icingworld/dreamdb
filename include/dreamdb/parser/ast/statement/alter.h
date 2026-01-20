#pragma once

#include <cstddef>
#include <string>
#include <variant>
#include <memory>

#include "dreamdb/parser/ast/statement/statement.h"
#include "dreamdb/parser/ast/statement/column_definition.h"

namespace dreamdb::parser::ast
{

/**
 * @brief ALTER ADD COLUMN 语句操作
 */
struct AstAlterAddColumn
{
    AstColumnDefinition column_definition;  // 列定义
};

/**
 * @brief ALTER DROP COLUMN 语句操作
 */
struct AstAlterDropColumn
{
    std::string column_name;  // 列名
};

/**
 * @brief ALTER MODIFY COLUMN 语句操作
 */
struct AstAlterModifyColumn
{
    AstColumnDefinition new_definition; // 新的列定义
};

/**
 * @brief ALTER RENAME COLUMN 语句操作
 */
struct AstAlterRenameColumn
{
    std::string old_name;  // 旧列名
    std::string new_name;  // 新列名
};

using AstAlterStatementOperation = std::variant<
    AstAlterAddColumn,         // ALTER ADD COLUMN 语句操作
    AstAlterDropColumn,        // ALTER DROP COLUMN 语句操作
    AstAlterModifyColumn,      // ALTER MODIFY COLUMN 语句操作
    AstAlterRenameColumn       // ALTER RENAME COLUMN 语句操作
>;

/**
 * @brief ALTER 语句
 */
class AstAlterStatement final : public AstStatement
{
public:
    AstAlterStatement(
        std::string collection_name,
        AstAlterStatementOperation operation,
        std::size_t line,
        std::size_t column
    );

    ~AstAlterStatement() noexcept override = default;

public:
    /**
     * @brief 创建 ALTER 语句
     * @param collection_name 集合名
     * @param operation ALTER 语句操作
     * @param line 行号
     * @param column 列号
     * @return ALTER 语句
     */
    static std::unique_ptr<AstAlterStatement> create(
        std::string collection_name,
        AstAlterStatementOperation operation,
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
     * @brief 获取 ALTER 语句操作
     * @return ALTER 语句操作
     */
    const AstAlterStatementOperation & operation() const noexcept;

    /**
     * @brief 接受语句访问者
     * @param visitor 语句访问者
     */
    void accept(AstStatementVisitor & visitor) const override;

private:
    std::string collection_name_;                  // 集合名
    AstAlterStatementOperation operation_;        // ALTER 语句操作
};

} // namespace dreamdb::parser::ast
