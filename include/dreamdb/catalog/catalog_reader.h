#pragma once

#include <string>
#include <vector>
#include <optional>

#include "dreamdb/common/ids.h"
#include "dreamdb/catalog/column_info.h"

namespace dreamdb::catalog
{

/**
 * @brief 数据库目录只读器
 */
class CatalogReader
{
public:
    virtual ~CatalogReader() noexcept = default;

public:
    /**
     * @brief 解析数据库名称
     * @param database_name 数据库名称
     * @return 数据库 ID
     */
    virtual std::optional<dreamdb::common::database_id_t> resolve_database(
        const std::string & database_name
    ) const = 0;

    /**
     * @brief 解析集合名称
     * @param database_id 数据库 ID
     * @param collection_name 集合名称
     * @return 集合 ID
     */
    virtual std::optional<dreamdb::common::collection_id_t> resolve_collection(
        const dreamdb::common::database_id_t database_id,
        const std::string & collection_name
    ) const = 0;

    /**
     * @brief 解析列名称
     * @param collection_id 集合 ID
     * @param column_name 列名称
     * @return 列信息
     */
    virtual std::optional<column_info_t> resolve_column(
        const dreamdb::common::collection_id_t collection_id,
        const std::string & column_name
    ) const = 0;

    /**
     * @brief 获取集合所有列信息
     * @param collection_id 集合 ID
     * @return 列信息列表
     * @details 该接口用于 SELECT * 投影类型和 INSERT 省略列名的场景
     */
    virtual std::vector<column_info_t> get_columns(
        const dreamdb::common::collection_id_t collection_id
    ) const = 0;
};

} // namespace dreamdb::catalog
