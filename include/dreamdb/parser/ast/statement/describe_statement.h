#pragma once

#include <cstddef>
#include <string>
#include <memory>

#include "dreamdb/parser/ast/statement/statement.h"

namespace dreamdb::parser::ast
{

/**
 * @brief DESCRIBE 语句节点
 */
class AstDescribeStatement final : public AstStatement
{
public:
    AstDescribeStatement(std::string collection_name, std::size_t line, std::size_t column);

    ~AstDescribeStatement() noexcept override = default;

public:
    /**
     * @brief 创建 DESCRIBE 语句
     * @param collection_name 集合名称
     * @param line 行号
     * @param column 列号
     * @return DESCRIBE 语句
     */
    static std::unique_ptr<AstDescribeStatement> create(
        std::string collection_name,
        std::size_t line,
        std::size_t column
    );

public:
    /**
     * @brief 获取集合名称
     * @return 集合名称
     */
    const std::string & collection_name() const noexcept;

    /**
     * @brief 接受语句访问者
     * @param visitor 语句访问者
     */
    void accept(AstStatementVisitor & visitor) const override;

private:
    std::string collection_name_;   // 集合名称
};

} // namespace dreamdb::parser::ast
