#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "dreamdb/schema/field.h"

namespace dreamdb
{

class Collection;

/**
 * @brief 集合管理器
 * @details 负责管理数据库中的所有集合
 */
class CollectionManager
{
public:
    CollectionManager() noexcept = default;

    CollectionManager(const CollectionManager &) = delete;

    CollectionManager(CollectionManager &&) noexcept = default;

    CollectionManager & operator=(const CollectionManager &) = delete;

    CollectionManager & operator=(CollectionManager &&) noexcept = default;

    ~CollectionManager() = default;

public:
    /** 集合管理接口 */

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
     * @return 如果成功删除返回 true，如果集合不存在返回 false
     */
    bool drop_collection(const std::string & name);

    /**
     * @brief 获取集合
     * @param name 集合名称
     * @return 集合指针，如果不存在返回 nullptr
     */
    Collection * get_collection(const std::string & name);

private:
    std::unordered_map<std::string, std::unique_ptr<Collection>> collections_;          // 集合映射表
};

} // namespace dreamdb
