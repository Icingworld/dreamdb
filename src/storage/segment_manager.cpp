#include "dreamdb/storage/segment_manager.h"

#include <algorithm>
#include <stdexcept>

namespace dreamdb
{

SegmentManager::SegmentManager(SegmentManager::StorageFactory factory)
    : storage_factory_(std::move(factory))
{
}

Segment * SegmentManager::get_or_create_segment(const std::string & collection_name, std::size_t field_count)
{
    if (!storage_factory_) {
        throw std::runtime_error("Storage factory is not configured");
    }

    auto & entry = collection_segments[collection_name];
    if (!entry.initialized) {
        entry.field_count = field_count;
        entry.initialized = true;
    }
    else if (entry.field_count != field_count) {
        throw std::invalid_argument("Field count mismatch for collection " + collection_name);
    }

    if (entry.active_segment && entry.active_segment->can_accept_writes()) {
        return entry.active_segment;
    }

    entry.active_segment = create_segment(entry, field_count);
    return entry.active_segment;
}

void SegmentManager::set_storage_factory(SegmentManager::StorageFactory factory)
{
    storage_factory_ = std::move(factory);
}

Segment * SegmentManager::seal_active_segment(const std::string & collection_name)
{
    auto it = collection_segments.find(collection_name);
    if (it == collection_segments.end()) {
        return nullptr;
    }

    auto * active = it->second.active_segment;
    if (active && active->can_accept_writes()) {
        active->seal();
        it->second.active_segment = nullptr;
        return active;
    }

    return nullptr;
}

std::vector<Segment *> SegmentManager::get_segments(const std::string & collection_name)
{
    std::vector<Segment *> result;

    auto it = collection_segments.find(collection_name);
    if (it == collection_segments.end()) {
        return result;
    }

    for (const auto & seg : it->second.segments) {
        result.push_back(seg.get());
    }

    return result;
}

Segment * SegmentManager::get_segment(const std::string & collection_name, SegmentId segment_id)
{
    return const_cast<Segment *>(find_segment_internal(collection_name, segment_id));
}

const Segment * SegmentManager::get_segment(const std::string & collection_name, SegmentId segment_id) const
{
    return find_segment_internal(collection_name, segment_id);
}

void SegmentManager::drop_segment(const std::string & collection_name, SegmentId segment_id)
{
    auto it = collection_segments.find(collection_name);
    if (it == collection_segments.end()) {
        return;
    }

    auto & segments = it->second.segments;
    segments.erase(
        std::remove_if(
            segments.begin(),
            segments.end(),
            [&](const std::unique_ptr<Segment> & seg) {
                if (seg->get_id() == segment_id) {
                    if (it->second.active_segment == seg.get()) {
                        it->second.active_segment = nullptr;
                    }
                    return true;
                }
                return false;
            }),
        segments.end());
}

Segment * SegmentManager::create_segment(CollectionSegments & entry, std::size_t field_count)
{
    auto storage = storage_factory_(field_count);
    auto segment = std::make_unique<Segment>(next_segment_id++, std::move(storage));
    auto * ptr = segment.get();
    entry.segments.push_back(std::move(segment));
    return ptr;
}

Segment * SegmentManager::find_segment_internal(const std::string & collection_name, SegmentId segment_id) const
{
    auto it = collection_segments.find(collection_name);
    if (it == collection_segments.end()) {
        return nullptr;
    }

    for (const auto & segment : it->second.segments) {
        if (segment->get_id() == segment_id) {
            return segment.get();
        }
    }

    return nullptr;
}

} // namespace dreamdb
