#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <memory>
#include <optional>

#include "dreamdb/parser/ast/ast_statement_node.h"

namespace dreamdb
{

class AstExpressionNode;

class UpdateAssignment
{
public:
    UpdateAssignment(const std::string & column_name, std::unique_ptr<AstExpressionNode> value);

    UpdateAssignment(const UpdateAssignment &) = delete;

    UpdateAssignment(UpdateAssignment &&) noexcept = default;

    UpdateAssignment & operator=(const UpdateAssignment &) = delete;

    UpdateAssignment & operator=(UpdateAssignment &&) noexcept = default;

    ~UpdateAssignment() noexcept;

public:
    /**
     * @brief 获取列名
     * @return 列名
     */
    const std::string & get_column_name() const;

    /**
     * @brief 获取值表达式
     * @return 值表达式
     */
    const AstExpressionNode & get_value() const noexcept;

    /**
     * @brief 是否存在列名
     * @return 是否存在列名
     */
    bool has_column_name() const noexcept;

    /**
     * @brief 是否存在值表达式
     * @return 是否存在值表达式
     */
    bool has_value() const noexcept;

private:
    std::optional<std::string> column_name_;        // 列名
    std::unique_ptr<AstExpressionNode> value_;      // 值表达式
};

/**
 * @brief UPDATE 语句节点
 */
class AstUpdateStatementNode : public AstStatementNode
{
public:
    AstUpdateStatementNode(std::size_t line = 0, std::size_t column = 0);

    AstUpdateStatementNode(const AstUpdateStatementNode &) = delete;

    AstUpdateStatementNode(AstUpdateStatementNode &&) noexcept = default;

    AstUpdateStatementNode & operator=(const AstUpdateStatementNode &) = delete;

    AstUpdateStatementNode & operator=(AstUpdateStatementNode &&) noexcept = default;

    ~AstUpdateStatementNode() noexcept override;

public:
    /**
     * @brief 设置集合名
     * @param collection_name 集合名
     */
    void set_collection_name(const std::string & collection_name);

    /**
     * @brief 添加一个赋值项
     * @param assignment 赋值项
     */
    void add_assignment(UpdateAssignment && assignment);

    /**
     * @brief 设置 WHERE 子句
     * @param where_clause WHERE 子句
     */
    void set_where_clause(std::unique_ptr<AstExpressionNode> where_clause) noexcept;

    /**
     * @brief 获取集合名
     * @return 集合名
     */
    const std::string & get_collection_name() const;

    /**
     * @brief 获取赋值列表
     * @return 赋值列表
     */
    const std::vector<UpdateAssignment> & get_assignments() const noexcept;

    /**
     * @brief 获取 WHERE 子句
     * @return WHERE 子句
     */
    const AstExpressionNode & get_where_clause() const;

    /**
     * @brief 是否存在集合名
     * @return 是否存在集合名
     */
    bool has_collection_name() const noexcept;

    /**
     * @brief 是否存在赋值项
     * @return 是否存在赋值项
     */
    bool has_assignments() const noexcept;

    /**
     * @brief 是否存在 WHERE 子句
     * @return 是否存在 WHERE 子句
     */
    bool has_where_clause() const noexcept;

private:
    std::optional<std::string> collection_name_;            // 集合名
    std::vector<UpdateAssignment> assignments_;             // SET 子句
    std::unique_ptr<AstExpressionNode> where_clause_;       // WHERE 子句
};

} // namespace dreamdb
