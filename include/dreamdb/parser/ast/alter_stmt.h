#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <optional>

#include "dreamdb/parser/ast/ast_node.h"
#include "dreamdb/parser/ast/column_definition.h"

namespace dreamdb
{

/**
 * @brief ALTER 语句节点
 * @details 表示 ALTER COLLECTION <collection_name> [ADD | DROP | MODIFY | RENAME] COLUMN <column_name> [<column_type> [constraints]]语句
 */
class AlterStmt : public AstNode
{
public:
    /**
     * @brief ALTER 类型
     */
    enum class AlterType : std::uint8_t
    {
        ADD_COLUMN,        // 添加字段
        DROP_COLUMN,       // 删除字段
        MODIFY_COLUMN,     // 修改字段
        RENAME_COLUMN      // 重命名字段
    };

public:
    AlterStmt(std::size_t line = 0, std::size_t column = 0);

    AlterStmt(const AlterStmt &) = delete;

    AlterStmt(AlterStmt &&) noexcept = default;

    AlterStmt & operator=(const AlterStmt &) = delete;

    AlterStmt & operator=(AlterStmt &&) noexcept = default;

    ~AlterStmt() noexcept = default;

public:
    /**
     * @brief 设置集合名称
     * @param collection_name 集合名称
     */
    void set_collection_name(const std::string & collection_name);

    /**
     * @brief 设置 ALTER 类型
     * @param type ALTER 类型
     */
    void set_alter_type(AlterType type);

    /**
     * @brief 设置字段名称
     * @param column_name 字段名称
     */
    void set_column_name(const std::string & column_name);

    /**
     * @brief 设置新字段名称
     * @param new_column_name 新字段名称
     */
    void set_new_column_name(const std::string & new_column_name);

    /**
     * @brief 设置字段类型
     * @param type 字段类型
     */
    void set_column_definition(ColumnDefinition && column_definition);

    /**
     * @brief 获取集合名称
     * @return 集合名称
     */
    const std::string & get_collection_name() const noexcept;

    /**
     * @brief 获取 ALTER 类型
     * @return ALTER 类型
     */
    AlterType get_alter_type() const noexcept;

    /**
     * @brief 获取字段名称
     * @return 字段名称
     */
    const std::string & get_column_name() const noexcept;

    /**
     * @brief 获取新字段名称
     * @return 新字段名称
     */
    const std::optional<std::string> & get_new_column_name() const noexcept;

    /**
     * @brief 获取字段定义
     * @return 字段定义
     */
    const std::optional<ColumnDefinition> & get_column_definition() const noexcept;

public:
    /**
     * @brief 调试字符串
     * @return 调试字符串
     */
    std::string debug_string() const override;

private:
    std::string collection_name_;                         // 集合名称
    AlterType type_;                                      // ALTER 类型
    std::string column_name_;                             // 字段名称
    std::optional<std::string> new_column_name_;          // 新字段名称
    std::optional<ColumnDefinition> column_definition_;   // 字段定义
};

} // namespace dreamdb
