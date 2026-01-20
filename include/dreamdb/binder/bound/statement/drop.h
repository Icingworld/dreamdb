#pragma once

#include <cstddef>
#include <string>
#include <variant>

#include "dreamdb/binder/bound/statement/statement.h"
#include "dreamdb/common/ids.h"

namespace dreamdb::binder::bound
{

/**
 * @brief 删除数据库操作
 */
struct BoundDropDatabase
{
    dreamdb::common::database_id_t database_id;  // 数据库 ID
};

/**
 * @brief 删除集合操作
 */
struct BoundDropCollection
{
    dreamdb::common::collection_id_t collection_id;  // 集合 ID
};

/**
 * @brief 删除索引操作
 */
struct BoundDropIndex
{
    dreamdb::common::index_id_t index_id;  // 索引 ID
};

/**
 * @brief 删除向量索引操作
 */
struct BoundDropVIndex
{
    dreamdb::common::vindex_id_t vindex_id;  // 向量索引 ID
};

using BoundDropOperation = std::variant<
    BoundDropDatabase,        // DROP DATABASE 操作
    BoundDropCollection,      // DROP COLLECTION 操作
    BoundDropIndex,           // DROP INDEX 操作
    BoundDropVIndex           // DROP VINDEX 操作
>;

/**
 * @brief 绑定后的 DROP 语句
 */
class BoundDropStatement final : public BoundStatement
{
public:
    explicit BoundDropStatement(BoundDropOperation operation, bool if_exists);

    ~BoundDropStatement() noexcept override = default;

public:
    /**
     * @brief 获取是否存在性检查
     * @return 是否存在性检查
     */
    bool if_exists() const noexcept;

    /**
     * @brief 获取删除操作
     * @return 删除操作
     */
    const BoundDropOperation & operation() const noexcept;

private:
    bool if_exists_;                // 是否存在性检查
    BoundDropOperation operation_;  // 删除操作
};

} // namespace dreamdb::binder::bound
