#pragma once

#include <unordered_map>

#include "dreamdb/storage/storage_base.h"
#include "dreamdb/common/mutation_result.h"

namespace dreamdb
{

/**
 * @brief 简单的内存存储实现
 * @details 使用 unordered_map 存储 Entity，主要用于原型阶段测试
 */
class MemoryStorage : public StorageBase
{
public:
    /**
     * @brief 构造函数
     * @param field_count 期望的字段数量
     */
    explicit MemoryStorage(std::size_t field_count) noexcept;

    ~MemoryStorage() override = default;

public:
    /**
     * @brief 插入实体
     * @param entity 要插入的实体
     * @return 操作结果
     * @note 如果实体 ID 已存在，操作失败
     */
    MutationResult insert(const Entity & entity) override;

    /**
     * @brief 按内部 ID 删除实体
     * @param id 内部 ID
     * @return 操作结果
     * @note 如果 ID 不存在，操作失败
     */
    MutationResult remove_by_id(std::int64_t id) override;

    /**
     * @brief 按内部 ID 更新实体
     * @param id 内部 ID
     * @param entity 新的实体数据
     * @return 操作结果
     * @note 如果 ID 不存在，操作失败
     */
    MutationResult update_by_id(std::int64_t id, std::vector<std::pair<std::size_t, FieldValue>> fields) override;

    /**
     * @brief 按内部 ID 获取实体
     * @param id 内部 ID
     * @return 实体指针，如果不存在返回 nullptr
     */
    std::unique_ptr<Entity> get_by_id(std::int64_t id) const override;

    /**
     * @brief 执行查询
     * @param query 查询对象，包含条件、排序和限制
     * @return 匹配的实体列表
     */
    std::vector<std::unique_ptr<Entity>> query(const Query & query) const override;

    /**
     * @brief 获取存储的实体数量
     * @return 实体数量
     */
    std::size_t size() const override;

    /**
     * @brief 检查是否为空
     * @return 如果为空返回 true
     */
    bool empty() const override;

    /**
     * @brief 检查指定 ID 是否存在
     * @param id 内部 ID
     * @return 如果存在返回 true
     */
    bool contains(std::int64_t id) const override;

    /**
     * @brief 清空所有数据
     * @return 操作结果
     */
    MutationResult clear() override;

private:
    std::size_t field_count_;                               // 期望的字段数量
    std::unordered_map<std::int64_t, Entity> entity_map_;   // 实体映射
};

} // namespace dreamdb

