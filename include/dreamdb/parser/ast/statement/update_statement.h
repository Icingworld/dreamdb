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
 * @brief UPDATE 赋值项
 */
struct AstUpdateAssignment
{
    AstUpdateAssignment(std::string column_name, std::unique_ptr<AstExpression> value);

    std::string column_name;                        // 列名
    std::unique_ptr<AstExpression> value;           // 值表达式
};

/**
 * @brief UPDATE 语句
 */
class AstUpdateStatement : public AstStatement
{
public:
    AstUpdateStatement(
        std::string collection_name,
        std::vector<AstUpdateAssignment> assignments,
        std::unique_ptr<AstExpression> where,
        std::size_t line,
        std::size_t column
    );

    ~AstUpdateStatement() noexcept override = default;

public:
    /**
     * @brief 创建 UPDATE 语句
     * @param collection_name 集合名
     * @param assignments 赋值列表
     * @param where WHERE 子句
     * @param line 行号
     * @param column 列号
     * @return UPDATE 语句
     */
    static std::unique_ptr<AstUpdateStatement> create(
        std::string collection_name,
        std::vector<AstUpdateAssignment> assignments,
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
     * @brief 获取指定索引的赋值项
     * @param index 索引
     * @return 赋值项
     */
    const AstUpdateAssignment & assignment_at(std::size_t index) const noexcept;

    /**
     * @brief 获取赋值项数量
     * @return 赋值项数量
     */
    std::size_t assignment_count() const noexcept;

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
    std::string collection_name_;                    // 集合名
    std::vector<AstUpdateAssignment> assignments_;   // SET 子句
    std::unique_ptr<AstExpression> where_;           // WHERE 子句
};

} // namespace dreamdb::parser::ast
