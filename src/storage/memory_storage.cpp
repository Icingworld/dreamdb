#include "dreamdb/storage/memory_storage.h"

namespace dreamdb
{

MemoryStorage::MemoryStorage(std::size_t field_count) noexcept
    : field_count_(field_count)
    , entity_map_()
{
}

MutationResult MemoryStorage::insert(const Entity & entity)
{
    // 查找是否有该 id 的实体
    auto it = entity_map_.find(entity.get_id());
    if (it != entity_map_.end()) {
        // 已经存在了，失败
        // 理论上不应该出现该情况
        return MutationResult::make_failure("Entity id already exists");
    }

    // 插入实体
    entity_map_[entity.get_id()] = entity;
    return MutationResult::make_success(1);
}

MutationResult MemoryStorage::remove_by_id(std::int64_t id)
{
    // 查找是否有该 id 的实体
    auto it = entity_map_.find(id);
    if (it == entity_map_.end()) {
        return MutationResult::make_failure("Entity id not found");
    }

    entity_map_.erase(it);
    return MutationResult::make_success(1);
}

std::unique_ptr<Entity> MemoryStorage::get_by_id(std::int64_t id) const
{
    auto it = entity_map_.find(id);
    if (it == entity_map_.end()) {
        return nullptr;
    }

    // 构造一个新的实体返回
    return std::make_unique<Entity>(it->second);
}

std::size_t MemoryStorage::size() const
{
    return entity_map_.size();
}

bool MemoryStorage::empty() const
{
    return entity_map_.empty();
}

bool MemoryStorage::contains(std::int64_t id) const
{
    return entity_map_.find(id) != entity_map_.end();
}

MutationResult MemoryStorage::clear()
{
    entity_map_.clear();
    return MutationResult::make_success(1);
}

} // namespace dreamdb
