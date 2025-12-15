#pragma once

#include <memory>
#include <string>
#include <unordered_map>

namespace dreamdb
{

class Database;

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

    ~DatabaseManager() = default;

public:
    /**
     * @brief 创建数据库
     * @param name 数据库名称
     * @param storage_factory 存储工厂函数，如果不提供则使用默认 factory
     * @return 创建的数据库指针，如果数据库已存在则返回 nullptr
     */
    Database * create_database(const std::string & name);

    /**
     * @brief 删除数据库
     * @param name 数据库名称
     * @return 是否删除成功
     */
    bool drop_database(const std::string & name);

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

private:
    std::string current_database_;                                          // 当前数据库名称
    std::unordered_map<std::string, std::unique_ptr<Database>> databases_;  // 数据库映射表
};

} // namespace dreamdb
