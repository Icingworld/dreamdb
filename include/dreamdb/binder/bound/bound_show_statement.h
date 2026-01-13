#pragma once

#include <cstddef>
#include <variant>
#include <optional>

#include "dreamdb/binder/bound/bound_statement.h"

namespace dreamdb
{

/**
 * @brief 显示数据库操作（绑定后）
 */
struct BoundShowDatabases
{
    // SHOW DATABASES 不需要任何参数
};

/**
 * @brief 显示集合操作（绑定后）
 */
struct BoundShowCollections
{
    std::optional<std::size_t> database_id;  // 数据库 ID（如果指定）
};

/**
 * @brief 显示索引操作（绑定后）
 */
struct BoundShowIndexes
{
    std::size_t collection_id;               // 集合 ID
    std::optional<std::size_t> database_id;  // 数据库 ID（如果指定）
};

/**
 * @brief 显示向量索引操作（绑定后）
 */
struct BoundShowVIndexes
{
    std::size_t collection_id;               // 集合 ID
    std::optional<std::size_t> database_id;  // 数据库 ID（如果指定）
};

/**
 * @brief 绑定后的 SHOW 语句
 */
class BoundShowStatement : public BoundStatement
{
public:
    explicit BoundShowStatement();

    ~BoundShowStatement() noexcept override = default;

public:
    std::variant<
        std::monostate,
        BoundShowDatabases,
        BoundShowCollections,
        BoundShowIndexes,
        BoundShowVIndexes
    > show_operation;  // 显示操作
};

} // namespace dreamdb
