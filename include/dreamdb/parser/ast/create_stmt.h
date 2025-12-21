#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <optional>

#include "dreamdb/parser/ast/ast_node.h"
#include "dreamdb/parser/ast/column_definition.h"


namespace dreamdb
{

/**
 * @brief CREATE 语句节点
 * @details 表示 CREATE {DATABASE | COLLECTION | INDEX} [IF NOT EXISTS] <object_name> [ON COLLECTION(<collection_name>) | (...)] 语句
 */
class CreateStmt : public AstNode
{
public:
    /**
     * @brief 对象类型枚举
     */
    enum class CreateType : std::uint8_t
    {
        DATABASE,    // 数据库
        COLLECTION,  // 集合
        INDEX        // 索引
    };

public:
    CreateStmt(std::size_t line = 0, std::size_t column = 0);

    CreateStmt(const CreateStmt &) = delete;

    CreateStmt(CreateStmt &&) noexcept = default;

    CreateStmt & operator=(const CreateStmt &) = delete;

    CreateStmt & operator=(CreateStmt &&) noexcept = default;

    ~CreateStmt() override = default;

public:
    /**
     * @brief 设置对象类型
     * @param type 对象类型
     */
    void set_create_type(CreateType type) noexcept;

    /**
     * @brief 设置对象名称
     * @param name 对象名称
     */
    void set_object_name(const std::string & name);

    /**
     * @brief 设置是否跳过存在性检查
     * @param is_if_not_exists 是否跳过存在性检查
     */
    void set_is_if_not_exists(bool is_if_not_exists);

    /**
     * @brief 添加列定义
     * @param column 列定义
     */
    void add_column_definition(ColumnDefinition && column);

    /**
     * @brief 设置集合名称
     * @param collection_name 集合名称
     */
    void set_collection_name(const std::string & collection_name);

    /**
     * @brief 获取对象类型
     * @return 对象类型
     */
    CreateType get_create_type() const noexcept;

    /**
     * @brief 获取对象名称
     * @return 对象名称
     */
    const std::string & get_object_name() const noexcept;

    /**
     * @brief 获取是否跳过存在性检查
     * @return 是否跳过存在性检查
     */
    bool get_is_if_not_exists() const noexcept;

    /**
     * @brief 获取所有列定义
     * @return 列定义列表
     */
    const std::optional<std::vector<ColumnDefinition>> & get_column_definitions() const noexcept;

    /**
     * @brief 获取集合名称
     * @return 集合名称
     */
    const std::optional<std::string> & get_collection_name() const noexcept;

public:
    /**
     * @brief 调试字符串
     * @return 调试字符串
     */
    std::string debug_string() const override;

private:
    CreateType create_type_;                             // 创建类型
    std::string object_name_;                            // 对象名称
    bool is_if_not_exists_;                              // 是否跳过存在性检查
    std::optional<std::vector<ColumnDefinition>> column_definitions_;   // 列定义列表（仅用于 COLLECTION）
    std::optional<std::string> collection_name_;         // 集合名称（仅用于 INDEX）
};

} // namespace dreamdb
