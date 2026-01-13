#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <variant>

#include "dreamdb/binder/bound/bound_statement.h"
#include "dreamdb/schema/field.h"
#include "dreamdb/common/type.h"

namespace dreamdb
{

/**
 * @brief 创建数据库操作（绑定后）
 */
struct BoundCreateDatabase
{
    std::string database_name;  // 数据库名称
};

/**
 * @brief 创建集合操作（绑定后）
 */
struct BoundCreateCollection
{
    std::string collection_name;          // 集合名称
    std::vector<Field> column_definitions;  // 列定义列表
};

/**
 * @brief 创建索引操作（绑定后）
 */
struct BoundCreateIndex
{
    std::size_t collection_id;            // 集合 ID
    std::string index_name;               // 索引名称
    std::vector<std::size_t> column_ids;  // 列 ID 列表
    IndexType index_type;                 // 索引类型
};

/**
 * @brief 创建向量索引操作（绑定后）
 */
struct BoundCreateVIndex
{
    std::size_t collection_id;            // 集合 ID
    std::string vindex_name;              // 向量索引名称
    std::size_t column_id;                // 列 ID
    VIndexType vindex_type;               // 向量索引类型
    std::vector<std::pair<std::string, std::string>> with_clauses;  // WITH 子句选项（键值对）
};

/**
 * @brief 绑定后的 CREATE 语句
 */
class BoundCreateStatement : public BoundStatement
{
public:
    explicit BoundCreateStatement();

    ~BoundCreateStatement() noexcept override = default;

public:
    bool if_not_exists;  // 是否跳过存在性检查
    std::variant<
        std::monostate,
        BoundCreateDatabase,
        BoundCreateCollection,
        BoundCreateIndex,
        BoundCreateVIndex
    > create_operation;  // 创建操作
};

} // namespace dreamdb
