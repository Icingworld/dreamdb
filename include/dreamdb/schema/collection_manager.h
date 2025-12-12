#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "dreamdb/schema/collection.h"

namespace dreamdb
{

class SegmentManager;

/**
 * @brief 集合管理器
 * @details 负责管理数据库中的所有集合（表），并拥有 SegmentManager 来统一管理段的生命周期
 */
class CollectionManager
{
public:
    CollectionManager() noexcept;

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

    /**
     * @brief 获取集合
     * @param name 集合名称
     * @return 集合指针，如果不存在返回 nullptr
     */
    const Collection * get_collection(const std::string & name) const;

    /**
     * @brief 检查集合是否存在
     * @param name 集合名称
     * @return 如果存在返回 true
     */
    bool has_collection(const std::string & name) const;

    /**
     * @brief 列出所有集合名称
     * @return 集合名称列表
     */
    std::vector<std::string> list_collections() const;

    /**
     * @brief 获取集合数量
     * @return 集合数量
     */
    std::size_t collection_count() const noexcept;

private:
    std::unique_ptr<SegmentManager> segment_manager_;                                   // 段管理器（拥有）
    std::unordered_map<std::string, std::unique_ptr<Collection>> collections_;          // 集合映射表
};

} // namespace dreamdb
