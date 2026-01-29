#pragma once

#include <string>
#include <vector>

#include "dreamdb/common/ids.h"
#include "dreamdb/schema/column_definition.h"

namespace dreamdb::schema
{

/**
 * @brief 集合元数据
 */
class Collection
{
public:
    Collection(
        dreamdb::common::collection_id_t id,
        std::string name,
        dreamdb::common::database_id_t database_id,
        std::vector<ColumnDefinition> columns
    );

public:
    /**
     * @brief 获取集合 ID
     * @return 集合 ID
     */
    dreamdb::common::collection_id_t id() const noexcept;

    /**
     * @brief 获取集合名称
     * @return 集合名称
     */
    const std::string & name() const noexcept;

    /**
     * @brief 获取数据库 ID
     * @return 数据库 ID
     */
    dreamdb::common::database_id_t database_id() const noexcept;

    /**
     * @brief 获取列定义
     * @return 列定义
     */
    const std::vector<ColumnDefinition> & columns() const noexcept;

private:
    // 基本信息
    dreamdb::common::collection_id_t id_;           // 集合 ID
    std::string name_;                              // 集合名称
    dreamdb::common::database_id_t database_id_;    // 数据库 ID

    // 列定义
    std::vector<ColumnDefinition> columns_;         // 列定义
};

} // namespace dreamdb::schema
