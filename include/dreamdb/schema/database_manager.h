#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "dreamdb/schema/database.h"
#include "dreamdb/catalog/catalog.h"

namespace dreamdb
{

/**
 * @brief 数据库管理器
 * @details DatabaseManager 为系统级上下文，管理存储层、元数据层
 */
class DatabaseManager
{
public:
    /**
     * @brief 构造函数
     */
    DatabaseManager() noexcept = default;

    DatabaseManager(const DatabaseManager &) = delete;

    DatabaseManager(DatabaseManager &&) noexcept = default;

    DatabaseManager & operator=(const DatabaseManager &) = delete;

    DatabaseManager & operator=(DatabaseManager &&) noexcept = default;

    ~DatabaseManager();

public:
    /**
     * @brief 创建数据库
     * @param name 数据库名称
     * @return 创建的数据库 ID
     */
    std::size_t create_database(const std::string & name);

    /**
     * @brief 删除数据库
     * @param id 数据库 ID
     * @return 是否删除成功
     */
    bool drop_database(std::size_t id);

    /**
     * @brief 设置当前数据库
     * @param id 数据库 ID
     * @details 不检查数据库是否存在，需确保数据库存在后再设置当前数据库
     */
    void set_current_database(std::size_t id);

    /**
     * @brief 获取当前数据库
     * @return 当前数据库指针
     * @details 不检查当前数据库是否存在，需确保当前数据库存在后再获取
     */
    Database * get_current_database();

    /**
     * @brief 检查数据库是否存在
     * @param id 数据库 ID
     * @return 是否存在
     */
    bool has_database(std::size_t id) const;

    /**
     * @brief 获取数据库列表
     * @return 数据库列表
     */
    std::vector<std::size_t> get_databases() const;

    /**
     * @brief 获取 Catalog 引用
     * @return Catalog 引用
     */
    const Catalog & get_catalog() const noexcept;

    /**
     * @brief 获取可变 Catalog 引用
     * @return Catalog 引用
     */
    Catalog & get_catalog() noexcept;

private:
    std::size_t current_database_id_;                                       // 当前数据库 ID
    std::unordered_map<std::size_t, std::unique_ptr<Database>> databases_;  // 数据库映射表
    std::unique_ptr<Catalog> catalog_;                                      // 元数据管理器
};

} // namespace dreamdb
