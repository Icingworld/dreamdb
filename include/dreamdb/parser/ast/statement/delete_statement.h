#pragma once

#include <cstddef>
#include <string>
#include <memory>

#include "dreamdb/parser/ast/statement/statement.h"

namespace dreamdb::parser::ast
{

class AstExpression;

/**
 * @brief DELETE 语句
 */
class AstDeleteStatement : public AstStatement
{
public:
    AstDeleteStatement(
        std::string collection_name,
        std::unique_ptr<AstExpression> where,
        std::size_t line,
        std::size_t column
    );

    ~AstDeleteStatement() noexcept override = default;

public:
    /**
     * @brief 创建 DELETE 语句
     * @param collection_name 集合名
     * @param where WHERE 子句
     * @param line 行号
     * @param column 列号
     * @return DELETE 语句
     */
    static std::unique_ptr<AstDeleteStatement> create(
        std::string collection_name,
        std::unique_ptr<AstExpression> where,
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
     * @brief 是否存在 WHERE 子句
     * @return 是否存在 WHERE 子句
     */
    bool has_where() const noexcept;

    /**
     * @brief 获取 WHERE 子句
     * @return WHERE 子句
     */
    const AstExpression * where() const noexcept;

    /**
     * @brief 获取 WHERE 子句
     * @return WHERE 子句
     */
    const AstExpression & where_ref() const noexcept;

    /**
     * @brief 接受语句访问者
     * @param visitor 语句访问者
     */
    void accept(AstStatementVisitor & visitor) const override;

private:
    std::string collection_name_;                   // 集合名
    std::unique_ptr<AstExpression> where_;          // WHERE 子句
};

} // namespace dreamdb::parser::ast
