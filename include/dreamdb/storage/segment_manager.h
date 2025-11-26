#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <functional>

#include "dreamdb/storage/segment.h"

namespace dreamdb
{

class SegmentManager
{
public:
    using StorageFactory = std::function<std::unique_ptr<StorageBase>(std::size_t field_count)>;

    SegmentManager() = default;

    explicit SegmentManager(StorageFactory factory);

    SegmentManager(const SegmentManager & other) = delete;

    SegmentManager(SegmentManager && other) noexcept = default;

    SegmentManager & operator=(const SegmentManager & other) = delete;

    SegmentManager & operator=(SegmentManager && other) noexcept = default;

    ~SegmentManager() = default;

public:
    /** 段管理接口 */

    /**
     * @brief 创建段
     * @param collection_name 集合名称
     * @param field_count 集合字段数量，用于构建存储
     * @return 段
     */
    Segment * get_or_create_segment(const std::string & collection_name, std::size_t field_count);

    /**
     * @brief 设置存储工厂
     * @param factory 工厂函数
     */
    void set_storage_factory(StorageFactory factory);

    /**
     * @brief 封存集合的当前活动段
     * @param collection_name 集合名称
     * @return 被封存的段，如果不存在则返回 nullptr
     */
    Segment * seal_active_segment(const std::string & collection_name);

    /**
     * @brief 获取集合的所有段
     * @param collection_name 集合名称
     * @return 段列表
     */
    std::vector<Segment *> get_segments(const std::string & collection_name);

    /**
     * @brief 获取段
     * @param collection_name 集合名称
     * @param segment_id 段 ID
     * @return 段
     */
    Segment * get_segment(const std::string & collection_name, SegmentId segment_id);

    /**
     * @brief 获取段
     * @param collection_name 集合名称
     * @param segment_id 段 ID
     * @return 段
     */
    const Segment * get_segment(const std::string & collection_name, SegmentId segment_id) const;

    /**
     * @brief 删除段
     * @param collection_name 集合名称
     * @param segment_id 段 ID
     */
    void drop_segment(const std::string & collection_name, SegmentId segment_id);

public:
    struct CollectionSegments
    {
        std::vector<std::unique_ptr<Segment>> segments;     // 段列表
        Segment * active_segment = nullptr;                 // 当前活动段
        std::size_t field_count = 0;                        // 字段数量（用于校验）
        bool initialized = false;                           // 是否已初始化
    };

    std::unordered_map<std::string, CollectionSegments> collection_segments;    // 集合名称 -> 段列表
    SegmentId next_segment_id = 1;                                              // 段 ID 生成器

private:
    StorageFactory storage_factory_;
    Segment * create_segment(CollectionSegments & entry, std::size_t field_count);
    Segment * find_segment_internal(const std::string & collection_name, SegmentId segment_id) const;
};

} // namespace dreamdb
