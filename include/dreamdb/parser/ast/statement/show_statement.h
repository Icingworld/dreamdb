#pragma once

#include <cstddef>
#include <string>
#include <variant>
#include <optional>
#include <memory>

#include "dreamdb/parser/ast/statement/statement.h"

namespace dreamdb::parser::ast
{

/**
 * @brief SHOW DATABASES 语句操作
 */
struct AstShowDatabases
{
};

/**
 * @brief SHOW COLLECTIONS 语句操作
 */
struct AstShowCollections
{
    std::optional<std::string> database_name;  // 可选的数据库名称
};

/**
 * @brief SHOW INDEXES 语句操作
 */
struct AstShowIndexes
{
    std::string collection_name;               // 集合名称
    std::optional<std::string> database_name;  // 可选的数据库名称
};

/**
 * @brief SHOW VINDEXES 语句操作
 */
struct AstShowVIndexes
{
    std::string collection_name;               // 集合名称
    std::optional<std::string> database_name;  // 可选的数据库名称
};

using AstShowStatementOperation = std::variant<
    AstShowDatabases,         // SHOW DATABASES 语句操作
    AstShowCollections,       // SHOW COLLECTIONS 语句操作
    AstShowIndexes,           // SHOW INDEXES 语句操作
    AstShowVIndexes           // SHOW VINDEXES 语句操作
>;

/**
 * @brief SHOW 语句
 */
class AstShowStatement final : public AstStatement
{
public:
    AstShowStatement(AstShowStatementOperation operation, std::size_t line, std::size_t column);

    ~AstShowStatement() noexcept override = default;

public:
    /**
     * @brief 创建 SHOW 语句
     * @param operation SHOW 语句操作
     * @param line 行号
     * @param column 列号
     * @return SHOW 语句
     */
    static std::unique_ptr<AstShowStatement> create(
        AstShowStatementOperation operation,
        std::size_t line,
        std::size_t column
    );

public:
    /**
     * @brief 获取 SHOW 语句操作
     * @return SHOW 语句操作
     */
    const AstShowStatementOperation & operation() const noexcept;

    /**
     * @brief 接受语句访问者
     * @param visitor 语句访问者
     */
    void accept(AstStatementVisitor & visitor) const override;

private:
    AstShowStatementOperation operation_;  // SHOW 语句操作
};

} // namespace dreamdb::parser::ast
