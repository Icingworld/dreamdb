#pragma once

#include <string>
#include <vector>
#include <variant>

#include "dreamdb/binder/bound/statement/statement.h"
#include "dreamdb/common/ids.h"
#include "dreamdb/schema/field.h"
#include "dreamdb/common/type.h"

namespace dreamdb::binder::bound
{

class BoundStatementVisitor;

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
    std::string collection_name;            // 集合名称
    std::vector<Field> column_definitions;  // 列定义列表 TODO: 需要重新设计一个结构，避免依赖 schema::Field
};

/**
 * @brief 创建索引操作
 */
struct BoundCreateIndex
{
    dreamdb::common::collection_id_t collection_id;            // 集合 ID
    std::string index_name;                                    // 索引名称
    std::vector<dreamdb::common::column_id_t> column_ids;      // 列 ID 列表
    IndexType index_type;                                      // 索引类型
};

/**
 * @brief 创建向量索引操作
 */
struct BoundCreateVIndex
{
    dreamdb::common::collection_id_t collection_id;            // 集合 ID
    std::string vindex_name;                                   // 向量索引名称
    dreamdb::common::column_id_t column_id;                    // 列 ID
    VIndexType vindex_type;                                    // 向量索引类型
    std::vector<std::pair<std::string, std::string>> with_options;  // WITH 子句选项 TODO: 需要一个结构来绑定
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
