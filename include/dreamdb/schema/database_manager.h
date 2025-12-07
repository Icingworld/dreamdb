#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

#include "dreamdb/schema/database.h"
#include "dreamdb/schema/collection_manager.h"

namespace dreamdb
{

class DatabaseManager
{
public:
    /**
     * @brief 构造函数
     * @param default_factory 默认存储工厂函数，如果不提供则使用 MemoryStorage
     */
    explicit DatabaseManager(CollectionManager::StorageFactory default_factory = nullptr);

    DatabaseManager(const DatabaseManager &) = delete;

    DatabaseManager(DatabaseManager &&) noexcept = default;

    DatabaseManager & operator=(const DatabaseManager &) = delete;

    DatabaseManager & operator=(DatabaseManager &&) noexcept = default;

    ~DatabaseManager() = default;

public:
    /**
     * @brief 创建数据库
     * @param name 数据库名称
     * @param storage_factory 存储工厂函数，如果不提供则使用默认 factory
     * @return 创建的数据库指针，如果数据库已存在则返回 nullptr
     */
    Database * create_database(
        const std::string & name, 
        CollectionManager::StorageFactory storage_factory = nullptr
    );

    /**
     * @brief 设置默认存储工厂
     * @param factory 存储工厂函数
     */
    void set_default_storage_factory(CollectionManager::StorageFactory factory);

    /**
     * @brief 删除数据库
     * @param name 数据库名称
     * @return 是否删除成功
     */
    bool drop_database(const std::string & name);

    /**
     * @brief 获取数据库
     * @param name 数据库名称
     * @return 数据库指针
     */
    Database * get_database(const std::string & name);

    /**
     * @brief 检查数据库是否存在
     * @param name 数据库名称
     * @return 是否存在
     */
    bool has_database(const std::string & name) const;

    /**
     * @brief 设置当前数据库
     * @param name 数据库名称
     * @details 不检查数据库是否存在，需确保数据库存在后再设置当前数据库
     */
    void set_current_database(const std::string & name);

    /**
     * @brief 获取当前数据库
     * @return 当前数据库指针
     * @details 不检查当前数据库是否存在，需确保当前数据库存在后再获取
     */
    Database * get_current_database();

    /**
     * @brief 获取当前数据库
     * @return 当前数据库指针
     * @details 不检查当前数据库是否存在，需确保当前数据库存在后再获取
     */
    const Database * get_current_database() const;

    /**
     * @brief 列出所有数据库名称
     * @return 数据库名称列表
     */
    std::vector<std::string> list_databases() const;

private:
    std::string current_database_;                                          // 当前数据库名称
    std::unordered_map<std::string, std::unique_ptr<Database>> databases_;  // 数据库映射表
    CollectionManager::StorageFactory default_factory_;                     // 默认存储工厂
};

} // namespace dreamdb
