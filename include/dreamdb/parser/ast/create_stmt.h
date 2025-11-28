#pragma once

#include <string>
#include <vector>
#include <memory>

#include "dreamdb/parser/ast/ast_node.h"
#include "dreamdb/common/type.h"

namespace dreamdb
{

/**
 * @brief 列定义
 */
class ColumnDefinition
{
public:
    ColumnDefinition();

    ColumnDefinition(const ColumnDefinition &) = delete;

    ColumnDefinition & operator=(const ColumnDefinition &) = delete;

    ColumnDefinition(ColumnDefinition &&) noexcept = default;

    ColumnDefinition & operator=(ColumnDefinition &&) noexcept = default;

    ~ColumnDefinition() = default;

public:
    /**
     * @brief 设置列名
     */
    void set_name(const std::string & name);

    /**
     * @brief 获取列名
     */
    const std::string & get_name() const noexcept;

    /**
     * @brief 设置字段类型
     */
    void set_type(FieldType type);

    /**
     * @brief 获取字段类型
     */
    FieldType get_type() const noexcept;

    /**
     * @brief 设置长度（用于 VARCHAR、CHAR 等）
     */
    void set_length(int length);

    /**
     * @brief 获取长度
     */
    int get_length() const noexcept;

    /**
     * @brief 设置精度（用于 FLOAT、DOUBLE 等）
     */
    void set_precision(int precision);

    /**
     * @brief 获取精度
     */
    int get_precision() const noexcept;

    /**
     * @brief 设置是否允许 NULL
     */
    void set_nullable(bool nullable);

    /**
     * @brief 是否允许 NULL
     */
    bool is_nullable() const noexcept;

    /**
     * @brief 设置是否为主键
     */
    void set_primary_key(bool primary);

    /**
     * @brief 是否为主键
     */
    bool is_primary_key() const noexcept;

    /**
     * @brief 设置是否自动递增
     */
    void set_auto_increment(bool auto_increment);

    /**
     * @brief 是否自动递增
     */
    bool is_auto_increment() const noexcept;

    /**
     * @brief 设置默认值表达式（AST 节点）
     */
    void set_default_value(std::unique_ptr<AstNode> expr);

    /**
     * @brief 获取默认值表达式
     */
    const AstNode * get_default_value() const noexcept;

    /**
     * @brief 是否有默认值
     */
    bool has_default_value() const noexcept;

private:
    std::string name;                               // 列名
    FieldType type;                                 // 字段类型
    int length;                                     // 长度
    int precision;                                  // 精度
    bool nullable;                                  // 是否允许 NULL
    bool primary_key;                               // 是否为主键
    bool auto_increment;                            // 是否自动递增
    std::unique_ptr<AstNode> default_value;         // 默认值表达式
};

/**
 * @brief CREATE 语句节点
 * @details 表示 CREATE COLLECTION/INDEX ... 语句
 */
class CreateStmt : public AstNode
{
public:
    /**
     * @brief 对象类型枚举
     */
    enum class ObjectType : std::uint8_t
    {
        COLLECTION,  // 集合
        INDEX        // TODO: 索引
    };

public:
    CreateStmt(std::size_t line = 0, std::size_t column = 0);

    CreateStmt(const CreateStmt &) = delete;

    CreateStmt & operator=(const CreateStmt &) = delete;

    CreateStmt(CreateStmt &&) noexcept = default;

    CreateStmt & operator=(CreateStmt &&) noexcept = default;

    ~CreateStmt() override = default;

public:
    /**
     * @brief 设置对象类型
     * @param type 对象类型
     */
    void set_object_type(ObjectType type);

    /**
     * @brief 获取对象类型
     * @return 对象类型
     */
    ObjectType get_object_type() const noexcept;

    /**
     * @brief 设置对象名称
     * @param name 对象名称
     */
    void set_object_name(const std::string & name);

    /**
     * @brief 获取对象名称
     * @return 对象名称
     */
    const std::string & get_object_name() const noexcept;

    /**
     * @brief 添加列定义
     * @param column 列定义
     */
    void add_column_definition(ColumnDefinition column);

    /**
     * @brief 获取所有列定义
     * @return 列定义列表
     */
    const std::vector<ColumnDefinition> & get_column_definitions() const noexcept;

    /**
     * @brief 获取列定义数量
     * @return 列定义数量
     */
    std::size_t get_column_count() const noexcept;

public:
    /**
     * @brief 调试字符串
     * @return 调试字符串
     */
    std::string debug_string() const override;

private:
    ObjectType object_type;                             // 对象类型
    std::string object_name;                            // 对象名称
    std::vector<ColumnDefinition> column_definitions;   // 列定义列表（仅用于 COLLECTION）
};

} // namespace dreamdb
