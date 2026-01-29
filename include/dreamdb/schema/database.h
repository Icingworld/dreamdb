#pragma once

#include <string>

#include "dreamdb/common/ids.h"

namespace dreamdb::schema
{

/**
 * @brief 数据库元数据
 */
class Database
{
public:
    Database(dreamdb::common::database_id_t id, std::string name);

public:
    /**
     * @brief 获取数据库 ID
     * @return 数据库 ID
     */
    dreamdb::common::database_id_t id() const noexcept;

    /**
     * @brief 获取数据库名称
     * @return 数据库名称
     */
    const std::string & name() const noexcept;

private:
    dreamdb::common::database_id_t id_;  // 数据库 ID
    std::string name_;                   // 数据库名称
};

} // namespace dreamdb::schema
