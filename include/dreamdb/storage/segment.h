#pragma once

#include <cstdint>
#include <memory>
#include <chrono>
#include <optional>

#include "dreamdb/storage/storage_base.h"

namespace dreamdb
{

using SegmentId = std::int64_t;

/**
 * @brief 段状态
 */
enum class SegmentStatus : std::uint8_t
{
    GROWING = 0,  // 正在增长，可以继续写入
    SEALED = 1,   // 已密封，不再接受新的写入
    FLUSHED = 2,  // 已刷新，数据已写入磁盘
    DROPPED = 3   // 已删除
};

/**
 * @brief 段
 */
class Segment
{
public:
    explicit Segment(SegmentId id, std::unique_ptr<StorageBase> storage);

    Segment(const Segment & other) = delete;

    Segment(Segment && other) noexcept = default;

    Segment & operator=(const Segment & other) = delete;

    Segment & operator=(Segment && other) noexcept = default;

    ~Segment() = default;

public:
    /** Segment 属性访问接口 */

    /**
     * @brief 获取段 ID
     * @return 段 ID
     */
    SegmentId get_id() const noexcept;

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
     * @brief 检查段是否可以继续写入
     * @return true 表示可以写入
     */
    bool can_accept_writes() const noexcept;

    /**
     * @brief 设置段状态
     * @param status 段状态
     */
    void set_status(SegmentStatus status) noexcept;

    /**
     * @brief 封存段
     * @throw std::runtime_error 如果段已经封存或状态非法
     */
    void seal();

    /**
     * @brief 标记段已刷新
     */
    void mark_flushed();

    /**
     * @brief 标记段已删除
     */
    void mark_dropped();

public:
    /** 统计信息访问接口 */

    /**
     * @brief 获取实体数量
     * @return 实体数量
     */
    std::size_t get_entity_count() const noexcept;

    /**
     * @brief 判断段是否为空
     * @return 是否为空
     */
    bool is_empty() const noexcept;

    /**
     * @brief 获取底层存储
     * @return 存储指针
     */
    StorageBase * get_storage() noexcept;

    /**
     * @brief 获取底层存储
     * @return 存储指针
     */
    const StorageBase * get_storage() const noexcept;

private:
    SegmentId id;                           // 段 ID
    std::unique_ptr<StorageBase> storage;   // 存储实现
    SegmentStatus status;                   // 段状态
    std::chrono::system_clock::time_point created_at;  // 创建时间
    std::optional<std::chrono::system_clock::time_point> sealed_at;  // 封存时间
};

} // namespace dreamdb
