#pragma once

#include <cstddef>
#include <vector>
#include <unordered_map>
#include <memory>

namespace dreamdb
{

class Segment;

/**
 * @brief 段管理器
 */
class SegmentManager
{
public:
    SegmentManager() noexcept;

    SegmentManager(const SegmentManager & other) = delete;

    SegmentManager(SegmentManager && other) noexcept = default;

    SegmentManager & operator=(const SegmentManager & other) = delete;

    SegmentManager & operator=(SegmentManager && other) noexcept = default;

    ~SegmentManager();

public:
    /** 段创建接口 */

    /**
     * @brief 创建新的段
     * @return 新创建的段的共享指针
     */
    std::shared_ptr<Segment> create_segment();

public:
    /** 段查询接口 */

    /**
     * @brief 获取所有段
     * @return 所有段的共享指针列表
     */
    std::vector<std::shared_ptr<Segment>> get_all_segments() const;

private:
    std::size_t next_id_;                                                 // 段 ID 生成器
    std::unordered_map<std::size_t, std::shared_ptr<Segment>> segments_;  // 段映射表
};

} // namespace dreamdb
