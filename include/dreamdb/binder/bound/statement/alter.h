#pragma once

#include <string>
#include <variant>

#include "dreamdb/binder/bound/statement/statement.h"
#include "dreamdb/common/ids.h"
#include "dreamdb/schema/field.h"

namespace dreamdb::binder::bound
{

class BoundStatementVisitor;

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
    dreamdb::common::column_id_t column_id;  // 列 ID
};

/**
 * @brief 修改字段操作（绑定后）
 */
struct BoundAlterModifyColumn
{
    dreamdb::common::column_id_t column_id;  // 列 ID
    Field new_definition;                     // 新的列定义（在执行时转换）
};

/**
 * @brief 重命名字段操作（绑定后）
 */
struct BoundAlterRenameColumn
{
    dreamdb::common::column_id_t column_id;  // 列 ID
    std::string new_name;                     // 新列名
};

using BoundAlterOperation = std::variant<
    BoundAlterAddColumn,      // ALTER ADD COLUMN 操作
    BoundAlterDropColumn,     // ALTER DROP COLUMN 操作
    BoundAlterModifyColumn,   // ALTER MODIFY COLUMN 操作
    BoundAlterRenameColumn    // ALTER RENAME COLUMN 操作
>;

/**
 * @brief 绑定后的 ALTER 语句
 */
class BoundAlterStatement final : public BoundStatement
{
public:
    explicit BoundAlterStatement(
        dreamdb::common::collection_id_t collection_id,
        BoundAlterOperation alter_operation
    );

    ~BoundAlterStatement() noexcept override = default;

public:
    /**
     * @brief 接受语句访问者
     * @param visitor 语句访问者
     */
    void accept(BoundStatementVisitor & visitor) const override;

public:
    /**
     * @brief 获取集合 ID
     * @return 集合 ID
     */
    dreamdb::common::collection_id_t collection_id() const noexcept;

    /**
     * @brief 获取 ALTER 操作
     * @return ALTER 操作
     */
    const BoundAlterOperation & alter_operation() const noexcept;

private:
    dreamdb::common::collection_id_t collection_id_;  // 集合 ID
    BoundAlterOperation alter_operation_;              // ALTER 操作
};

} // namespace dreamdb::binder::bound
