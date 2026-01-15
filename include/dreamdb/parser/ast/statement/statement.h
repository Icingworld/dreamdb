#pragma once

#include <cstddef>

#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb::parser::ast
{

class AstStatementVisitor;

/**
 * @brief 语句节点基类
 * @details 每个具体的语句节点都继承自此类
 */
class AstStatement : public AstNode
{
protected:
    AstStatement(std::size_t line, std::size_t column) noexcept;

public:
    ~AstStatement() noexcept override = default;

public:
    /**
     * @brief 接受语句访问者
     * @param visitor 语句访问者
     */
    virtual void accept(AstStatementVisitor & visitor) const = 0;
};

} // namespace dreamdb::parser::ast
