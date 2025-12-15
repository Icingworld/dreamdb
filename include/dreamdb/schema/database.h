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
    Database(const std::string & name);

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
     * @brief 创建集合
     * @param name 集合名称
     * @param schema 字段定义列表
     * @return 创建的集合指针，如果集合已存在或创建失败则返回 nullptr
     */
    Collection * create_collection(const std::string & name, const std::vector<Field> & schema);

    /**
     * @brief 删除集合
     * @param name 集合名称
     * @return 删除结果
     */
    bool drop_collection(const std::string & name);

    /**
     * @brief 获取集合
     * @param name 集合名称
     * @return 获取的集合指针，如果不存在则返回 nullptr
     */
    Collection * get_collection(const std::string & name);

private:
    std::string name_;              // 数据库名称
    std::unique_ptr<CollectionManager> collection_manager_; // 集合管理器
};

} // namespace dreamdb
