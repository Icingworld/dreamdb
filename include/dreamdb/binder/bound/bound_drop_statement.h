#pragma once

#include <cstddef>
#include <string>
#include <variant>

#include "dreamdb/binder/bound/bound_statement.h"

namespace dreamdb
{

/**
 * @brief 删除数据库操作（绑定后）
 */
struct BoundDropDatabase
{
    std::size_t database_id;  // 数据库 ID
};

/**
 * @brief 删除集合操作（绑定后）
 */
struct BoundDropCollection
{
    std::size_t collection_id;  // 集合 ID
};

/**
 * @brief 删除索引操作（绑定后）
 */
struct BoundDropIndex
{
    std::size_t collection_id;  // 集合 ID
    std::string index_name;     // 索引名称
};

/**
 * @brief 删除向量索引操作（绑定后）
 */
struct BoundDropVIndex
{
    std::size_t collection_id;  // 集合 ID
    std::string vindex_name;    // 向量索引名称
};

/**
 * @brief 绑定后的 DROP 语句
 */
class BoundDropStatement : public BoundStatement
{
public:
    explicit BoundDropStatement();

    ~BoundDropStatement() noexcept override = default;

public:
    bool if_exists;  // 是否存在性检查
    std::variant<
        std::monostate,
        BoundDropDatabase,
        BoundDropCollection,
        BoundDropIndex,
        BoundDropVIndex
    > drop_operation;  // 删除操作
};

} // namespace dreamdb
