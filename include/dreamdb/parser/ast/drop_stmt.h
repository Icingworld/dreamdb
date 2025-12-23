#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb
{

/**
 * @brief DROP 语句节点
 * @details 表示 DROP [DATABASE | COLLECTION | INDEX | VINDEX] <object_name> 语句
 */
class DropStmt : public AstNode
{
public:
    /**
     * @brief 对象类型枚举
     */
    enum class DropType : std::uint8_t
    {
        DATABASE,    // 数据库
        COLLECTION,  // 集合
        INDEX,       // 索引
        VINDEX,      // 向量索引
    };

    /**
     * @brief 将对象类型转换为字符串
     * @param drop_type 对象类型
     * @return 字符串
     */
    static std::string drop_type_to_string(DropType drop_type);

public:
    DropStmt(std::size_t line = 0, std::size_t column = 0);

    DropStmt(const DropStmt &) = delete;

    DropStmt(DropStmt &&) noexcept = default;

    DropStmt & operator=(const DropStmt &) = delete;

    DropStmt & operator=(DropStmt &&) noexcept = default;

    ~DropStmt() override = default;

public:
    /**
     * @brief 设置对象类型
     * @param type 对象类型
     */
    void set_drop_type(DropType drop_type);

    /**
     * @brief 获取对象类型
     * @return 对象类型
     */
    DropType get_drop_type() const noexcept;

    /**
     * @brief 设置对象名称
     * @param name 对象名称
     */
    void set_object_name(const std::string & object_name);

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
    DropType drop_type_;            // 删除类型
    std::string object_name_;       // 对象名称
};

} // namespace dreamdb
