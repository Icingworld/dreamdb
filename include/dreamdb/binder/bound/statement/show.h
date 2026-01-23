#pragma once

#include <variant>
#include <optional>

#include "dreamdb/binder/bound/statement/statement.h"
#include "dreamdb/common/ids.h"

namespace dreamdb::binder::bound
{

class BoundStatementVisitor;

/**
 * @brief 显示数据库操作
 */
struct BoundShowDatabases
{
};

/**
 * @brief 显示集合操作
 */
struct BoundShowCollections
{
    std::optional<dreamdb::common::database_id_t> database_id;    // 数据库 ID
};

/**
 * @brief 显示索引操作
 */
struct BoundShowIndexes
{
    dreamdb::common::collection_id_t collection_id;               // 集合 ID
};

/**
 * @brief 显示向量索引操作
 */
struct BoundShowVIndexes
{
    dreamdb::common::collection_id_t collection_id;               // 集合 ID
};

using BoundShowOperation = std::variant<
    BoundShowDatabases,          // SHOW DATABASES 操作
    BoundShowCollections,        // SHOW COLLECTIONS 操作
    BoundShowIndexes,            // SHOW INDEXES 操作
    BoundShowVIndexes            // SHOW VINDEXES 操作
>;

/**
 * @brief 绑定后的 SHOW 语句
 */
class BoundShowStatement final : public BoundStatement
{
public:
    explicit BoundShowStatement(BoundShowOperation operation);

    ~BoundShowStatement() noexcept override = default;

public:
    /**
     * @brief 接受语句访问者
     * @param visitor 语句访问者
     */
    void accept(BoundStatementVisitor & visitor) const override;

public:
    /**
     * @brief 获取显示操作
     * @return 显示操作
     */
    const BoundShowOperation & operation() const noexcept;

private:
    BoundShowOperation operation_;  // 显示操作
};

} // namespace dreamdb::binder::bound
