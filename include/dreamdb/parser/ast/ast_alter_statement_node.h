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

/**
 * @brief 添加字段操作
 */
class AstAlterAddColumn
{
public:
    AstAlterAddColumn(ColumnDefinition && column);

    AstAlterAddColumn(const AstAlterAddColumn &) = delete;

    AstAlterAddColumn(AstAlterAddColumn &&) noexcept = default;

    AstAlterAddColumn & operator=(const AstAlterAddColumn &) = delete;

    AstAlterAddColumn & operator=(AstAlterAddColumn &&) noexcept = default;

    ~AstAlterAddColumn() noexcept = default;

public:
    /**
     * @brief 获取列定义
     * @return 列定义
     */
    const ColumnDefinition & get_column() const noexcept;

private:
    ColumnDefinition column_;  // 列定义
};

/**
 * @brief 删除字段操作
 */
class AstAlterDropColumn
{
public:
    AstAlterDropColumn(const std::string & column_name);

    AstAlterDropColumn(const AstAlterDropColumn &) = delete;

    AstAlterDropColumn(AstAlterDropColumn &&) noexcept = default;

    AstAlterDropColumn & operator=(const AstAlterDropColumn &) = delete;

    AstAlterDropColumn & operator=(AstAlterDropColumn &&) noexcept = default;

    ~AstAlterDropColumn() noexcept = default;

public:
    /**
     * @brief 获取列名
     * @return 列名
     */
    const std::string & get_column_name() const noexcept;

private:
    std::string column_name_;  // 列名
};

/**
 * @brief 修改字段操作
 */
class AstAlterModifyColumn
{
public:
    AstAlterModifyColumn(const std::string & column_name, ColumnDefinition && new_definition);

    AstAlterModifyColumn(const AstAlterModifyColumn &) = delete;

    AstAlterModifyColumn(AstAlterModifyColumn &&) noexcept = default;

    AstAlterModifyColumn & operator=(const AstAlterModifyColumn &) = delete;

    AstAlterModifyColumn & operator=(AstAlterModifyColumn &&) noexcept = default;

    ~AstAlterModifyColumn() noexcept = default;

public:
    /**
     * @brief 获取列名
     * @return 列名
     */
    const std::string & get_column_name() const noexcept;

    /**
     * @brief 获取新的列定义
     * @return 新的列定义
     */
    const ColumnDefinition & get_new_definition() const noexcept;

private:
    std::string column_name_;      // 列名
    ColumnDefinition new_definition_;  // 新的列定义
};

/**
 * @brief 重命名字段操作
 */
class AstAlterRenameColumn
{
public:
    AstAlterRenameColumn(const std::string & old_name, const std::string & new_name);

    AstAlterRenameColumn(const AstAlterRenameColumn &) = delete;

    AstAlterRenameColumn(AstAlterRenameColumn &&) noexcept = default;

    AstAlterRenameColumn & operator=(const AstAlterRenameColumn &) = delete;

    AstAlterRenameColumn & operator=(AstAlterRenameColumn &&) noexcept = default;

    ~AstAlterRenameColumn() noexcept = default;

public:
    /**
     * @brief 获取旧列名
     * @return 旧列名
     */
    const std::string & get_old_name() const noexcept;

    /**
     * @brief 获取新列名
     * @return 新列名
     */
    const std::string & get_new_name() const noexcept;

private:
    std::string old_name_;  // 旧列名
    std::string new_name_;  // 新列名
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
    void set_alter_type(AstAlterType alter_type);

    /**
     * @brief 设置添加字段操作
     * @param op 添加字段操作
     */
    void set_add_column(AstAlterAddColumn && op);

    /**
     * @brief 设置删除字段操作
     * @param op 删除字段操作
     */
    void set_drop_column(AstAlterDropColumn && op);

    /**
     * @brief 设置修改字段操作
     * @param op 修改字段操作
     */
    void set_modify_column(AstAlterModifyColumn && op);

    /**
     * @brief 设置重命名字段操作
     * @param op 重命名字段操作
     */
    void set_rename_column(AstAlterRenameColumn && op);

    /**
     * @brief 获取集合名称
     * @return 集合名称
     */
    const std::string & get_collection_name() const;

    /**
     * @brief 获取 ALTER 类型
     * @return ALTER 类型
     */
    AstAlterType get_alter_type() const noexcept;

    /**
     * @brief 获取添加字段操作
     * @return 添加字段操作
     */
    const AstAlterAddColumn & get_add_column() const;

    /**
     * @brief 获取删除字段操作
     * @return 删除字段操作
     */
    const AstAlterDropColumn & get_drop_column() const;

    /**
     * @brief 获取修改字段操作
     * @return 修改字段操作
     */
    const AstAlterModifyColumn & get_modify_column() const;

    /**
     * @brief 获取重命名字段操作
     * @return 重命名字段操作
     */
    const AstAlterRenameColumn & get_rename_column() const;

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

    /**
     * @brief 是否存在添加字段操作
     * @return 是否存在添加字段操作
     */
    bool has_add_column() const noexcept;

    /**
     * @brief 是否存在删除字段操作
     * @return 是否存在删除字段操作
     */
    bool has_drop_column() const noexcept;

    /**
     * @brief 是否存在修改字段操作
     * @return 是否存在修改字段操作
     */
    bool has_modify_column() const noexcept;

    /**
     * @brief 是否存在重命名字段操作
     * @return 是否存在重命名字段操作
     */
    bool has_rename_column() const noexcept;

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
