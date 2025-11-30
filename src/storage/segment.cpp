#include "dreamdb/storage/segment.h"

#include <stdexcept>

namespace dreamdb
{

Segment::Segment(SegmentId id, std::unique_ptr<StorageBase> storage)
    : id(id)
    , storage(std::move(storage))
    , status(SegmentStatus::GROWING)
    , created_at(std::chrono::system_clock::now())
{
    if (!this->storage) {
        throw std::invalid_argument("Segment requires a valid storage implementation");
    }
}

SegmentId Segment::get_id() const noexcept
{
    return id;
}

SegmentStatus Segment::get_status() const noexcept
{
    return status;
}

std::chrono::system_clock::time_point Segment::get_created_at() const noexcept
{
    return created_at;
}

std::optional<std::chrono::system_clock::time_point> Segment::get_sealed_at() const noexcept
{
    return sealed_at;
}

bool Segment::can_accept_writes() const noexcept
{
    return status == SegmentStatus::GROWING;
}

void Segment::set_status(SegmentStatus new_status) noexcept
{
    status = new_status;
    if (status == SegmentStatus::SEALED) {
        sealed_at = std::chrono::system_clock::now();
    }
}

void Segment::seal()
{
    if (status != SegmentStatus::GROWING) {
        throw std::runtime_error("Segment is not in a writable state");
    }

    set_status(SegmentStatus::SEALED);
}

void Segment::mark_flushed()
{
    set_status(SegmentStatus::FLUSHED);
}

void Segment::mark_dropped()
{
    set_status(SegmentStatus::DROPPED);
}

std::size_t Segment::get_entity_count() const noexcept
{
    return storage ? storage->size() : 0;
}

bool Segment::is_empty() const noexcept
{
    return !storage || storage->empty();
}

StorageBase * Segment::get_storage() noexcept
{
    return storage.get();
}

const StorageBase * Segment::get_storage() const noexcept
{
    return storage.get();
}

} // namespace dreamdb

