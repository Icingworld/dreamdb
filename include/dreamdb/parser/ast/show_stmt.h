#pragma once

#include <cstdint>
#include <cstddef>
#include <string>

#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb
{

/**
 * @brief SHOW 语句节点
 * @details 表示 SHOW [DATABASES | COLLECTIONS | INDEXES] 语句
 */
class ShowStmt : public AstNode
{
public:
    /**
     * @brief 对象类型枚举
     */
    enum class ShowType : std::uint8_t
    {
        DATABASES,    // 数据库
        COLLECTIONS,  // 集合
        INDEXES       // 索引
    };

public:
    ShowStmt(std::size_t line = 0, std::size_t column = 0);

    ShowStmt(const ShowStmt &) = delete;

    ShowStmt(ShowStmt &&) noexcept = default;

    ShowStmt & operator=(const ShowStmt &) = delete;

    ShowStmt & operator=(ShowStmt &&) noexcept = default;

    ~ShowStmt() override = default;

public:
    /**
     * @brief 设置显示类型
     * @param show_type 显示类型
     */
    void set_show_type(ShowType show_type) noexcept;

    /**
     * @brief 设置对象名称
     * @param object_name 对象名称
     */
    void set_object_name(const std::string & object_name);

    /**
     * @brief 获取显示类型
     * @return 显示类型
     */
    ShowType get_show_type() const noexcept;

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
    ShowType show_type_;        // 显示类型
    std::string object_name_;   // 对象名称
};

} // namespace dreamdb
