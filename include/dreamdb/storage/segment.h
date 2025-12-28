#pragma once

#include <cstdint>
#include <memory>
#include <chrono>
#include <optional>

#include "dreamdb/common/type.h"
#include "dreamdb/common/mutation_result.h"
#include "dreamdb/schema/entity.h"
#include "dreamdb/query/query.h"

namespace dreamdb
{

class StorageBase;
class Collection;

/**
 * @brief 段
 */
class Segment
{
public:
    explicit Segment(std::int64_t id);

    Segment(const Segment & other) = delete;

    Segment(Segment && other) noexcept = default;

    Segment & operator=(const Segment & other) = delete;

    Segment & operator=(Segment && other) noexcept = default;

    ~Segment();

public:
    /** Segment 属性访问接口 */

    /**
     * @brief 设置段状态
     * @param status 段状态
     */
    void set_status(SegmentStatus status) noexcept;

    /**
     * @brief 获取段 ID
     * @return 段 ID
     */
    std::int64_t get_id() const noexcept;

    /**
     * @brief 获取段状态
     * @return 段状态
     */
    SegmentStatus get_status() const noexcept;

    /**
     * @brief 获取创建时间
     * @return 创建时间
     */
    std::chrono::system_clock::time_point get_created_at() const noexcept;

    /**
     * @brief 获取封存时间
     * @return 如果已经封存则返回时间，否则返回空
     */
    std::optional<std::chrono::system_clock::time_point> get_sealed_at() const noexcept;

    /**
     * @brief 封存段
     * @throw std::runtime_error 如果段已经封存或状态非法
     */
    void seal();

public:
    /** 实体操作接口 */

    /**
     * @brief 插入实体
     * @param entity 要插入的实体
     * @return 操作结果
     * @note 如果实体 ID 已存在，操作失败
     */
    MutationResult insert(const Entity & entity);

    /**
     * @brief 按内部 ID 删除实体
     * @param id 内部 ID
     * @return 操作结果
     * @note 如果 ID 不存在，操作失败
     */
    MutationResult remove_by_id(std::int64_t id);

    /**
     * @brief 按内部 ID 更新实体
     * @param id 内部 ID
     * @param fields 要更新的字段
     * @return 操作结果
     * @note 如果 ID 不存在，操作失败
     */
    MutationResult update_by_id(std::int64_t id, std::vector<std::pair<std::size_t, FieldValue>> fields);
 
    /**
     * @brief 按内部 ID 获取实体
     * @param id 内部 ID
     * @return 实体指针，如果不存在返回 nullptr
     */
    std::unique_ptr<Entity> get_by_id(std::int64_t id) const;
 
    /**
     * @brief 执行查询
     * @param query 查询对象，包含条件、排序和限制
     * @param collection 集合对象，用于创建评估上下文
     * @return 匹配的实体列表
     */
    std::vector<std::unique_ptr<Entity>> query(const Query & query, const Collection * collection) const;

public:
    /** 统计信息访问接口 */

    /**
     * @brief 获取实体数量
     * @return 实体数量
     */
    std::size_t size() const noexcept;

    /**
     * @brief 判断段是否为空
     * @return 是否为空
     */
    bool empty() const noexcept;

private:
    std::int64_t id_;                                                   // 段 ID
    SegmentStatus status_;                                              // 段状态
    std::unique_ptr<StorageBase> storage_;                              // 存储实现
    std::chrono::system_clock::time_point created_at_;                  // 创建时间
    std::optional<std::chrono::system_clock::time_point> sealed_at_;    // 封存时间
};

} // namespace dreamdb
