#pragma once

#include <cstdint>
#include <cstddef>

#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb::parser::ast
{

class AstStatementVisitor;

/**
 * @brief 语句节点类型
 */
enum class AstStatementNodeType : std::uint8_t
{
    Select,              // 查询语句
    Insert,              // 插入语句
    Delete,              // 删除语句
    Update,              // 更新语句
    Create,              // 创建语句
    Drop,                // 删除语句
    Use,                 // 使用语句
    Alter,               // 修改语句
    Show,                // 显示语句
    Describe             // 描述语句
};

/**
 * @brief 语句节点基类
 * @details 每个具体的语句节点都继承自此类
 */
class AstStatement : public AstNode
{
protected:
    AstStatement(AstStatementNodeType type, std::size_t line, std::size_t column) noexcept;

public:
    ~AstStatement() noexcept override = default;

public:
    /**
     * @brief 获取语句节点类型
     * @return 语句节点类型
     */
    AstStatementNodeType type() const noexcept;

    /**
     * @brief 接受语句访问者
     * @param visitor 语句访问者
     */
    virtual void accept(AstStatementVisitor & visitor) const = 0;

private:
    AstStatementNodeType type_;    // 语句节点类型
};

} // namespace dreamdb::parser::ast
