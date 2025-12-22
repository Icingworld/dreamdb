#pragma once

#include <cstdint>
#include <cstddef>
#include <string>

#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb
{

/**
 * @brief SHOW 语句节点
 * @details 表示 SHOW [DATABASES | COLLECTIONS | INDEXES | VINDEXES] [FROM collection_name] 语句
 */
class ShowStmt : public AstNode
{
public:
    /**
     * @brief 显示类型枚举
     */
    enum class ShowType : std::uint8_t
    {
        DATABASES,    // 数据库
        COLLECTIONS,  // 集合
        INDEXES,      // 标量索引
        VINDEXES      // 向量索引
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
     * @brief 设置集合名称
     * @param collection_name 集合名称
     */
    void set_collection_name(const std::string & collection_name);

    /**
     * @brief 获取显示类型
     * @return 显示类型
     */
    ShowType get_show_type() const noexcept;

    /**
     * @brief 获取集合名称
     * @return 集合名称
     */
    const std::string & get_collection_name() const noexcept;

public:
    /**
     * @brief 调试字符串
     * @return 调试字符串
     */
    std::string debug_string() const override;

private:
    ShowType show_type_;            // 显示类型
    std::string collection_name_;   // 集合名称
};

} // namespace dreamdb
