#include "dreamdb/storage/segment.h"

#include <stdexcept>

#include "dreamdb/storage/memory_storage.h"

namespace dreamdb
{

Segment::~Segment() = default;

Segment::Segment(std::int64_t id)
    : id_(id)
    , status_(SegmentStatus::GROWING)
    , storage_(std::make_unique<MemoryStorage>())
    , created_at_(std::chrono::system_clock::now())
    , sealed_at_(std::nullopt)
{
}

void Segment::set_status(SegmentStatus status) noexcept
{
    status_ = status;
}

std::int64_t Segment::get_id() const noexcept
{
    return id_;
}

SegmentStatus Segment::get_status() const noexcept
{
    return status_;
}

std::chrono::system_clock::time_point Segment::get_created_at() const noexcept
{
    return created_at_;
}

std::optional<std::chrono::system_clock::time_point> Segment::get_sealed_at() const noexcept
{
    return sealed_at_;
}

MutationResult Segment::insert(const Entity & entity)
{
    return storage_->insert(entity);
}

MutationResult Segment::remove_by_id(std::int64_t id)
{
    return storage_->remove_by_id(id);
}

MutationResult Segment::update_by_id(std::int64_t id, std::vector<std::pair<std::size_t, FieldValue>> fields)
{
    return storage_->update_by_id(id, fields);
}

std::unique_ptr<Entity> Segment::get_by_id(std::int64_t id) const
{
    return storage_->get_by_id(id);
}

std::vector<std::unique_ptr<Entity>> Segment::query(const Query & query) const
{
    return storage_->query(query);
}

std::size_t Segment::size() const noexcept
{
    return storage_->size();
}

bool Segment::empty() const noexcept
{
    return storage_->empty();
}

void Segment::seal()
{
    if (status_ != SegmentStatus::GROWING) {
        throw std::runtime_error("Segment is not growing");
    }
    status_ = SegmentStatus::SEALED;
    sealed_at_ = std::chrono::system_clock::now();

    // TODO: 将数据写入磁盘
}

} // namespace dreamdb

