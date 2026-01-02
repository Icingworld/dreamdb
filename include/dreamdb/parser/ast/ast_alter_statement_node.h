#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <optional>
#include <variant>

#include "dreamdb/parser/ast/ast_statement_node.h"
#include "dreamdb/parser/ast/column_definition.h"

namespace dreamdb
{

/**
* @brief ALTER 类型
*/
enum class AstAlterType : std::uint8_t
{
    AST_ALTER_UNKNOWN,          // 未知类型
    AST_ALTER_ADD_COLUMN,       // 添加字段
    AST_ALTER_DROP_COLUMN,      // 删除字段
    AST_ALTER_MODIFY_COLUMN,    // 修改字段
    AST_ALTER_RENAME_COLUMN     // 重命名字段
};

struct AstAlterAddColumn
{
    ColumnDefinition column;
};

struct AstAlterDropColumn
{
    std::string column_name;
};

struct AstAlterModifyColumn
{
    std::string column_name;
    ColumnDefinition new_definition;
};

struct AstAlterRenameColumn
{
    std::string old_name;
    std::string new_name;
};

/**
 * @brief ALTER 语句节点
 */
class AstAlterStatementNode : public AstStatementNode
{
public:
    AstAlterStatementNode(std::size_t line = 0, std::size_t column = 0);

    AstAlterStatementNode(const AstAlterStatementNode &) = delete;

    AstAlterStatementNode(AstAlterStatementNode &&) noexcept = default;

    AstAlterStatementNode & operator=(const AstAlterStatementNode &) = delete;

    AstAlterStatementNode & operator=(AstAlterStatementNode &&) noexcept = default;

    ~AstAlterStatementNode() noexcept override = default;

public:
    /**
     * @brief 设置集合名称
     * @param collection_name 集合名称
     */
    void set_collection_name(const std::string & collection_name);

    /**
     * @brief 设置 ALTER 类型
     * @param alter_type ALTER 类型
     */
    void set_alter_type(AstAlterType alter_type) noexcept;

    /**
     * @brief 设置添加字段操作
     * @param op 添加字段操作
     */
    void set_add_column(AstAlterAddColumn && op) noexcept;

    /**
     * @brief 设置删除字段操作
     * @param op 删除字段操作
     */
    void set_drop_column(AstAlterDropColumn && op) noexcept;

    /**
     * @brief 设置修改字段操作
     * @param op 修改字段操作
     */
    void set_modify_column(AstAlterModifyColumn && op) noexcept;

    /**
     * @brief 设置重命名字段操作
     * @param op 重命名字段操作
     */
    void set_rename_column(AstAlterRenameColumn && op) noexcept;

    /**
     * @brief 获取集合名称
     * @return 集合名称
     */
    const std::string & get_collection_name() const noexcept;

    /**
     * @brief 获取 ALTER 类型
     * @return ALTER 类型
     */
    AstAlterType get_alter_type() const noexcept;

    /**
     * @brief 获取添加字段操作
     * @return 添加字段操作
     */
    const AstAlterAddColumn & get_add_column() const noexcept;

    /**
     * @brief 获取删除字段操作
     * @return 删除字段操作
     */
    const AstAlterDropColumn & get_drop_column() const noexcept;

    /**
     * @brief 获取修改字段操作
     * @return 修改字段操作
     */
    const AstAlterModifyColumn & get_modify_column() const noexcept;

    /**
     * @brief 获取重命名字段操作
     * @return 重命名字段操作
     */
    const AstAlterRenameColumn & get_rename_column() const noexcept;

    /**
     * @brief 是否存在集合名称
     * @return 是否存在集合名称
     */
    bool has_collection_name() const noexcept;

    /**
     * @brief 是否存在 ALTER 操作
     * @return 是否存在 ALTER 操作
     */
    bool has_alter_operation() const noexcept;

private:
    std::optional<std::string> collection_name_;               // 集合名称
    AstAlterType alter_type_;                                  // ALTER 类型
    std::variant<
        std::monostate,
        AstAlterAddColumn,
        AstAlterDropColumn,
        AstAlterModifyColumn,
        AstAlterRenameColumn
    > alter_operation_;                                        // ALTER 操作
};

} // namespace dreamdb
