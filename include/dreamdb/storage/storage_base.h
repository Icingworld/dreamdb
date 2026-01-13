#pragma once

#include <cstdint>
#include <vector>
#include <utility>
#include <memory>

#include "dreamdb/schema/entity.h"
#include "dreamdb/common/mutation_result.h"

namespace dreamdb
{

class Collection;

/**
 * @brief 存储基类
 * @details 存储基类定义了存储的抽象接口，所有存储实现都应该继承此类
 */
class StorageBase
{
public:
    virtual ~StorageBase() = default;

public:
    /** CRUD 操作 */

    /**
     * @brief 插入实体
     * @param entity 要插入的实体
     * @return 操作结果
     * @note 如果实体 ID 已存在，操作失败
     */
    virtual MutationResult insert(const Entity & entity) = 0;

    /**
     * @brief 按内部 ID 删除实体
     * @param id 内部 ID
     * @return 操作结果
     * @note 如果 ID 不存在，操作失败
     */
    virtual MutationResult remove_by_id(std::size_t id) = 0;

    /**
     * @brief 按内部 ID 更新实体
     * @param id 内部 ID
     * @param entity 新的实体数据
     * @return 操作结果
     * @note 如果 ID 不存在，操作失败
     */
    virtual MutationResult update_by_id(std::size_t id, std::vector<std::pair<std::size_t, FieldValue>> fields) = 0;

public:
    /** 查询操作 */

    /**
     * @brief 按内部 ID 获取实体
     * @param id 内部 ID
     * @return 实体指针，如果不存在返回 nullptr
     */
    virtual std::unique_ptr<Entity> get_by_id(std::size_t id) const = 0;

    /**
     * @brief 获取所有实体
     * @return 所有实体的列表
     * @note 用于全表扫描，由物理计划层处理过滤、排序和限制
     */
    virtual std::vector<std::unique_ptr<Entity>> get_all_entities() const = 0;

public:
    /** 统计信息 */

    /**
     * @brief 获取存储的实体数量
     * @return 实体数量
     */
    virtual std::size_t size() const = 0;

    /**
     * @brief 检查是否为空
     * @return 如果为空返回 true
     */
    virtual bool empty() const = 0;

    /**
     * @brief 检查指定 ID 是否存在
     * @param id 内部 ID
     * @return 如果存在返回 true
     */
    virtual bool contains(std::size_t id) const = 0;

public:
    /** 批量操作 */

    /**
     * @brief 清空所有数据
     * @return 操作结果
     */
    virtual MutationResult clear() = 0;
};

} // namespace dreamdb
