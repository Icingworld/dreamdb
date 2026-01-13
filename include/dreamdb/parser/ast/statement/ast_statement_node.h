#pragma once

#include <cstdint>
#include <cstddef>

#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb
{

/**
 * @brief 语句节点类型
 */
enum class AstStatementNodeType : std::uint8_t
{
    AST_STATEMENT_SELECT,              // 查询语句
    AST_STATEMENT_INSERT,              // 插入语句
    AST_STATEMENT_DELETE,              // 删除语句
    AST_STATEMENT_UPDATE,              // 更新语句
    AST_STATEMENT_CREATE,              // 创建语句
    AST_STATEMENT_DROP,                // 删除语句
    AST_STATEMENT_USE,                 // 使用语句
    AST_STATEMENT_ALTER,               // 修改语句
    AST_STATEMENT_SHOW,                // 显示语句
    AST_STATEMENT_DESCRIBE             // 描述语句
};

/**
 * @brief 语句节点基类
 * @details 每个具体的语句节点都继承自此类
 */
class AstStatementNode : public AstNode
{
protected:
    AstStatementNode(AstStatementNodeType statement_type, std::size_t line = 0, std::size_t column = 0) noexcept;

public:
    AstStatementNode(const AstStatementNode &) = delete;
    
    AstStatementNode(AstStatementNode &&) noexcept = default;

    AstStatementNode & operator=(const AstStatementNode &) = delete;

    AstStatementNode & operator=(AstStatementNode &&) noexcept = default;

    ~AstStatementNode() noexcept override = default;

public:
    /**
     * @brief 获取语句节点类型
     * @return 语句节点类型
     */
    AstStatementNodeType get_statement_type() const noexcept;

private:
    AstStatementNodeType statement_type_;    // 语句节点类型
};

} // namespace dreamdb
