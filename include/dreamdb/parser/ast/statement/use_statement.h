#pragma once

#include <cstddef>
#include <string>
#include <memory>

#include "dreamdb/parser/ast/statement/statement.h"

namespace dreamdb::parser::ast
{

/**
 * @brief USE 语句节点
 */
class AstUseStatement : public AstStatement
{
public:
    AstUseStatement(std::string database_name, std::size_t line, std::size_t column);

    ~AstUseStatement() noexcept override = default;

public:
    /**
     * @brief 创建 USE 语句
     * @param database_name 数据库名称
     * @param line 行号
     * @param column 列号
     * @return USE 语句
     */
    static std::unique_ptr<AstUseStatement> create(
        std::string database_name,
        std::size_t line,
        std::size_t column
    );

public:
    /**
     * @brief 获取数据库名称
     * @return 数据库名称
     */
    const std::string & database_name() const noexcept;

    /**
     * @brief 接受语句访问者
     * @param visitor 语句访问者
     */
    void accept(AstStatementVisitor & visitor) const override;

private:
    std::string database_name_;  // 数据库名称
};

} // namespace dreamdb::parser::ast
