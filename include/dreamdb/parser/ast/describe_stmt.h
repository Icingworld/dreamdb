#pragma once

#include <cstddef>
#include <string>

#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb
{

/**
 * @brief DESCRIBE 语句节点
 * @details 表示 [DESCRIBE | DESC] <collection_name> 语句
 */
class DescribeStmt : public AstNode
{
public:
    DescribeStmt(std::size_t line = 0, std::size_t column = 0);

    DescribeStmt(const DescribeStmt &) = delete;

    DescribeStmt(DescribeStmt &&) noexcept = default;

    DescribeStmt & operator=(const DescribeStmt &) = delete;

    DescribeStmt & operator=(DescribeStmt &&) noexcept = default;

    ~DescribeStmt() override = default;

public:
    /**
     * @brief 设置集合名称
     * @param collection_name 集合名称
     */
    void set_collection_name(const std::string & collection_name);

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
    std::string collection_name_;   // 集合名称
};

} // namespace dreamdb
