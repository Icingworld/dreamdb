#pragma once

#include <string>

#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb
{

/**
 * @brief DROP 语句节点
 * @details 表示 DROP COLLECTION/INDEX ... 语句
 */
class DropStmt : public AstNode
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
    DropStmt(std::size_t line = 0, std::size_t column = 0);

    DropStmt(const DropStmt &) = delete;

    DropStmt & operator=(const DropStmt &) = delete;

    DropStmt(DropStmt &&) noexcept = default;

    DropStmt & operator=(DropStmt &&) noexcept = default;

    ~DropStmt() override = default;

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

public:
    /**
     * @brief 调试字符串
     * @return 调试字符串
     */
    std::string debug_string() const override;

private:
    ObjectType object_type;         // 对象类型
    std::string object_name;        // 对象名称
};

} // namespace dreamdb