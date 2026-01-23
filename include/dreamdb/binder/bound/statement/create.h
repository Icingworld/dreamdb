#pragma once

#include <string>
#include <vector>
#include <variant>

#include "dreamdb/binder/bound/statement/statement.h"
#include "dreamdb/binder/bound/statement/column_definition.h"
#include "dreamdb/common/type.h"
#include "dreamdb/common/ids.h"

namespace dreamdb::binder::bound
{

class BoundStatementVisitor;

/**
 * @brief 向量索引 WITH 子句选项
 */
struct BoundVIndexWithOption
{
    std::optional<std::size_t> m;                         // HNSW 索引的 M 参数
    std::optional<std::size_t> nlist;                     // IVF_FLAT 索引的 nlist 参数
    std::optional<std::size_t> ef_construction;           // HNSW 索引的 ef_construction 参数
    std::optional<dreamdb::common::MetricType> metric;    // 距离度量方式
};

/**
 * @brief 创建数据库操作
 */
struct BoundCreateDatabase
{
    std::string database_name;  // 数据库名称
};

/**
 * @brief 创建集合操作
 */
struct BoundCreateCollection
{
    std::string collection_name;                              // 集合名称
    std::vector<BoundColumnDefinition> column_definitions;    // 列定义列表
};

/**
 * @brief 创建索引操作
 */
struct BoundCreateIndex
{
    dreamdb::common::collection_id_t collection_id;            // 集合 ID
    std::string index_name;                                    // 索引名称
    std::vector<dreamdb::common::column_id_t> column_ids;      // 列 ID 列表
    common::IndexType index_type;                              // 索引类型
};

/**
 * @brief 创建向量索引操作
 */
struct BoundCreateVIndex
{
    dreamdb::common::collection_id_t collection_id;            // 集合 ID
    std::string vindex_name;                                   // 向量索引名称
    dreamdb::common::column_id_t column_id;                    // 列 ID
    common::VIndexType vindex_type;                            // 向量索引类型
    BoundVIndexWithOption with_options;                        // WITH 子句选项
};

using BoundCreateOperation = std::variant<
    BoundCreateDatabase,         // CREATE DATABASE 操作
    BoundCreateCollection,       // CREATE COLLECTION 操作
    BoundCreateIndex,            // CREATE INDEX 操作
    BoundCreateVIndex            // CREATE VINDEX 操作
>;

/**
 * @brief 绑定后的 CREATE 语句
 */
class BoundCreateStatement final : public BoundStatement
{
public:
    explicit BoundCreateStatement(
        bool if_not_exists,
        BoundCreateOperation create_operation
    );

    ~BoundCreateStatement() noexcept override = default;

public:
    /**
     * @brief 接受语句访问者
     * @param visitor 语句访问者
     */
    void accept(BoundStatementVisitor & visitor) const override;

public:
    /**
     * @brief 是否跳过存在性检查
     * @return 是否跳过存在性检查
     */
    bool if_not_exists() const noexcept;

    /**
     * @brief 获取创建操作
     * @return 创建操作
     */
    const BoundCreateOperation & create_operation() const noexcept;

private:
    bool if_not_exists_;                     // 是否跳过存在性检查
    BoundCreateOperation create_operation_;  // 创建操作
};

} // namespace dreamdb::binder::bound
