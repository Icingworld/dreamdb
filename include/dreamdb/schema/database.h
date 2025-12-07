#pragma once

#include <string>
#include <memory>

#include "dreamdb/schema/collection_manager.h"

namespace dreamdb
{

/**
 * @brief 数据库类
 */
class Database
{
public:
    Database(const std::string & name, CollectionManager::StorageFactory storage_factory);

    Database(const Database &) = delete;

    Database(Database &&) noexcept = default;

    Database & operator=(const Database &) = delete;

    Database & operator=(Database &&) noexcept = default;

    ~Database() = default;

public:
    /**
     * @brief 获取数据库名称
     * @return 数据库名称
     */
    const std::string & get_name() const;

    /**
     * @brief 获取集合管理器
     * @return 集合管理器
     */
    CollectionManager & get_collection_manager();

    /**
     * @brief 获取集合管理器（常量版本）
     * @return 集合管理器
     */
    const CollectionManager & get_collection_manager() const;

private:
    std::string name_;              // 数据库名称
    std::unique_ptr<CollectionManager> collection_manager_; // 集合管理器
};

} // namespace dreamdb
