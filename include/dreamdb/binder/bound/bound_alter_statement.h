#pragma once

#include <cstddef>
#include <string>
#include <variant>

#include "dreamdb/binder/bound/bound_statement.h"
#include "dreamdb/schema/field.h"

namespace dreamdb
{

/**
 * @brief 添加字段操作（绑定后）
 */
struct BoundAlterAddColumn
{
    Field column_definition;  // 列定义（在执行时转换）
};

/**
 * @brief 删除字段操作（绑定后）
 */
struct BoundAlterDropColumn
{
    std::size_t column_id;  // 列 ID
};

/**
 * @brief 修改字段操作（绑定后）
 */
struct BoundAlterModifyColumn
{
    std::size_t column_id;   // 列 ID
    Field new_definition;    // 新的列定义（在执行时转换）
};

/**
 * @brief 重命名字段操作（绑定后）
 */
struct BoundAlterRenameColumn
{
    std::size_t column_id;  // 列 ID
    std::string new_name;   // 新列名
};

/**
 * @brief 绑定后的 ALTER 语句
 */
class BoundAlterStatement : public BoundStatement
{
public:
    explicit BoundAlterStatement();

    ~BoundAlterStatement() noexcept override = default;

public:
    std::size_t collection_id;  // 集合 ID
    std::variant<
        std::monostate,
        BoundAlterAddColumn,
        BoundAlterDropColumn,
        BoundAlterModifyColumn,
        BoundAlterRenameColumn
    > alter_operation;  // ALTER 操作
};

} // namespace dreamdb
