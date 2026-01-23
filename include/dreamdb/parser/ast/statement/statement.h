#pragma once

#include <cstddef>
#include <cstdint>

#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb::parser::ast
{

class AstStatementVisitor;

/**
 * @brief Ast 语句类型
 */
enum class AstStatementType : std::uint8_t
{
    Alter,
    Create,
    Delete,
    Describe,
    Drop,
    Insert,
    Select,
    Show,
    Update,
    Use
};

/**
 * @brief 语句节点基类
 * @details 每个具体的语句节点都继承自此类
 */
class AstStatement : public AstNode
{
protected:
    AstStatement(AstStatementType statement_type, std::size_t line, std::size_t column) noexcept;

public:
    ~AstStatement() noexcept override = default;

public:
    /**
     * @brief 获取语句类型
     * @return 语句类型
     */
    AstStatementType statement_type() const noexcept;

    /**
     * @brief 接受语句访问者
     * @param visitor 语句访问者
     */
    virtual void accept(AstStatementVisitor & visitor) const = 0;

private:
    AstStatementType statement_type_;    // 语句类型
};

} // namespace dreamdb::parser::ast
