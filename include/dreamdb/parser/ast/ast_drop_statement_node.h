#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <optional>

#include "dreamdb/parser/ast/ast_statement_node.h"

namespace dreamdb
{

/**
 * @brief DROP 类型枚举
 */
enum class AstDropType : std::uint8_t
{
    AST_DROP_UNKNOWN,     // 未知
    AST_DROP_DATABASE,    // 数据库
    AST_DROP_COLLECTION,  // 集合
    AST_DROP_INDEX,       // 索引
    AST_DROP_VINDEX,      // 向量索引
};

/**
 * @brief DROP 语句节点
 */
class AstDropStatementNode : public AstStatementNode
{
public:
    AstDropStatementNode(std::size_t line = 0, std::size_t column = 0);

    AstDropStatementNode(const AstDropStatementNode &) = delete;

    AstDropStatementNode(AstDropStatementNode &&) noexcept = default;

    AstDropStatementNode & operator=(const AstDropStatementNode &) = delete;

    AstDropStatementNode & operator=(AstDropStatementNode &&) noexcept = default;

    ~AstDropStatementNode() noexcept override = default;

public:
    /**
     * @brief 设置 DROP 类型
     * @param drop_type DROP 类型
     */
    void set_drop_type(AstDropType drop_type) noexcept;

    /**
     * @brief 设置对象名称
     * @param object_name 对象名称
     */
    void set_object_name(const std::string & object_name);

    /**
     * @brief 设置集合名称
     * @param collection_name 集合名称
     */
    void set_collection_name(const std::string & collection_name);

    /**
     * @brief 获取 DROP 类型
     * @return DROP 类型
     */
    AstDropType get_drop_type() const noexcept;

    /**
     * @brief 获取对象名称
     * @return 对象名称
     */
    const std::string & get_object_name() const noexcept;

    /**
     * @brief 获取集合名称
     * @return 集合名称
     */
    const std::string & get_collection_name() const noexcept;

    /**
     * @brief 是否存在对象名称
     * @return 是否存在对象名称
     */
    bool has_object_name() const noexcept;

    /**
     * @brief 是否存在集合名称
     * @return 是否存在集合名称
     */
    bool has_collection_name() const noexcept;

private:
    AstDropType drop_type_;                        // 删除类型
    std::optional<std::string> object_name_;       // 对象名称
    std::optional<std::string> collection_name_;   // 集合名称，用于索引和向量索引
};

} // namespace dreamdb
