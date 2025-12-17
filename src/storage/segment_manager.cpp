#include "dreamdb/storage/segment_manager.h"

#include <algorithm>
#include <stdexcept>

namespace dreamdb
{

SegmentManager::SegmentManager() noexcept
    : next_id_(1)
{
}

std::shared_ptr<Segment> SegmentManager::create_segment()
{
    auto segment = std::make_shared<Segment>(next_id_++);
    segments_[segment->get_id()] = segment;
    return segment;
}

std::vector<std::shared_ptr<Segment>> SegmentManager::get_all_segments() const
{
    std::vector<std::shared_ptr<Segment>> segments;
    for (const auto & [_, segment] : segments_) {
        segments.push_back(segment);
    }
    return segments;
}

} // namespace dreamdb
