#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <optional>

#include "dreamdb/parser/ast/ast_statement_node.h"

namespace dreamdb
{

/**
 * @brief 显示类型枚举
 */
enum class AstShowType : std::uint8_t
{
    AST_SHOW_UNKNOWN,      // 未知
    AST_SHOW_DATABASES,    // 数据库
    AST_SHOW_COLLECTIONS,  // 集合
    AST_SHOW_INDEXES,      // 标量索引
    AST_SHOW_VINDEXES      // 向量索引
};

/**
 * @brief SHOW 语句节点
 */
class AstShowStatementNode : public AstStatementNode
{
public:
    AstShowStatementNode(std::size_t line = 0, std::size_t column = 0);

    AstShowStatementNode(const AstShowStatementNode &) = delete;

    AstShowStatementNode(AstShowStatementNode &&) noexcept = default;

    AstShowStatementNode & operator=(const AstShowStatementNode &) = delete;

    AstShowStatementNode & operator=(AstShowStatementNode &&) noexcept = default;

    ~AstShowStatementNode() noexcept override = default;

public:
    /**
     * @brief 设置显示类型
     * @param show_type 显示类型
     */
    void set_show_type(AstShowType show_type) noexcept;

    /**
     * @brief 设置集合名称
     * @param collection_name 集合名称
     */
    void set_collection_name(const std::string & collection_name);

    /**
     * @brief 获取显示类型
     * @return 显示类型
     */
    AstShowType get_show_type() const noexcept;

    /**
     * @brief 获取集合名称
     * @return 集合名称
     */
    const std::string & get_collection_name() const noexcept;

    /**
     * @brief 是否存在集合名称
     * @return 是否存在集合名称
     */
    bool has_collection_name() const noexcept;

private:
    AstShowType show_type_;                        // 显示类型
    std::optional<std::string> collection_name_;   // 集合名称
};

} // namespace dreamdb
