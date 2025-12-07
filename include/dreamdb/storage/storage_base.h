#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include "dreamdb/schema/entity.h"
#include "dreamdb/common/type.h"

namespace dreamdb
{

/**
 * @brief 存储基类
 * @details 存储基类定义了存储的抽象接口，所有存储实现都应该继承此类
 */
class StorageBase
{
public:
    virtual ~StorageBase() = default;

public:
    /** 基础操作 */

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
     */
    virtual MutationResult remove_by_id(std::int64_t id) = 0;

    /**
     * @brief 按字段条件删除实体
     * @param field_index 字段索引
     * @param value 字段值（用于匹配）
     * @return 操作结果（包含删除的记录数）
     * @note 可能删除多条记录
     */
    virtual MutationResult remove_by_field(std::size_t field_index, const FieldValue & value) = 0;

    /**
     * @brief 按内部 ID 更新实体
     * @param id 内部 ID
     * @param entity 新的实体数据
     * @return 操作结果
     * @note 如果 ID 不存在，操作失败
     */
    virtual MutationResult update_by_id(std::int64_t id, const Entity & entity) = 0;

    /**
     * @brief 按字段条件更新或插入实体
     * @param field_index 用于匹配的字段索引
     * @param value 用于匹配的字段值
     * @param entity 要更新或插入的实体
     * @return 操作结果
     * @note 如果找到匹配记录则更新，否则插入
     */
    virtual MutationResult upsert_by_field(std::size_t field_index, const FieldValue & value, const Entity & entity) = 0;

public:
    /** 查询操作 */

    /**
     * @brief 按内部 ID 获取实体
     * @param id 内部 ID
     * @return 实体指针，如果不存在返回 nullptr
     */
    virtual std::unique_ptr<Entity> get_by_id(std::int64_t id) const = 0;

    /**
     * @brief 按字段条件查询实体
     * @param field_index 字段索引
     * @param value 字段值（用于匹配）
     * @return 匹配的实体列表
     */
    virtual std::vector<std::unique_ptr<Entity>> query_by_field(std::size_t field_index, const FieldValue & value) const = 0;

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
    virtual bool contains(std::int64_t id) const = 0;

public:
    /** 批量操作 */

    /**
     * @brief 批量插入实体
     * @param entities 实体列表
     * @return 操作结果（包含成功插入的数量）
     */
    virtual MutationResult insert_batch(const std::vector<Entity> & entities) = 0;

    /**
     * @brief 清空所有数据
     * @return 操作结果
     */
    virtual MutationResult clear() = 0;
};

} // namespace dreamdb
