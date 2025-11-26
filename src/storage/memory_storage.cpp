#include "dreamdb/storage/memory_storage.h"

#include <stdexcept>

namespace dreamdb
{

namespace
{

MutationResult make_error(const std::string & message)
{
    MutationResult result;
    result.success = false;
    result.error_message = message;
    return result;
}

MutationResult make_success(std::size_t affected = 1)
{
    MutationResult result;
    result.success = true;
    result.affected_count = affected;
    return result;
}

} // namespace

MemoryStorage::MemoryStorage(std::size_t field_count)
    : expected_field_count_(field_count)
{
}

MutationResult MemoryStorage::insert(const Entity & entity)
{
    if (expected_field_count_ != 0 && entity.field_count() != expected_field_count_) {
        return make_error("Entity field count mismatch");
    }

    const auto id = entity.get_id();
    if (entities_.find(id) != entities_.end()) {
        return make_error("Entity already exists");
    }

    entities_.emplace(id, clone_entity(entity));
    return make_success();
}

MutationResult MemoryStorage::remove_by_id(std::int64_t id)
{
    std::size_t removed = entities_.erase(id);
    if (removed == 0) {
        return make_error("Entity not found");
    }
    return make_success(removed);
}

MutationResult MemoryStorage::remove_by_field(std::size_t field_index, const FieldValue & value)
{
    if (expected_field_count_ != 0 && field_index >= expected_field_count_) {
        return make_error("Field index out of range");
    }

    std::vector<std::int64_t> ids;
    ids.reserve(entities_.size());
    for (const auto & [id, entity] : entities_) {
        if (match_field_value(entity, field_index, value)) {
            ids.push_back(id);
        }
    }

    for (auto id : ids) {
        entities_.erase(id);
    }

    if (ids.empty()) {
        return make_error("No entity matches the field condition");
    }

    return make_success(ids.size());
}

MutationResult MemoryStorage::update_by_id(std::int64_t id, const Entity & entity)
{
    if (expected_field_count_ != 0 && entity.field_count() != expected_field_count_) {
        return make_error("Entity field count mismatch");
    }

    auto it = entities_.find(id);
    if (it == entities_.end()) {
        return make_error("Entity not found");
    }

    it->second = clone_entity(entity);
    it->second.set_id(id);
    return make_success();
}

MutationResult MemoryStorage::upsert_by_field(std::size_t field_index, const FieldValue & value, const Entity & entity)
{
    if (expected_field_count_ != 0 && entity.field_count() != expected_field_count_) {
        return make_error("Entity field count mismatch");
    }

    for (auto & [id, stored_entity] : entities_) {
        if (match_field_value(stored_entity, field_index, value)) {
            stored_entity = clone_entity(entity);
            stored_entity.set_id(id);
            return make_success();
        }
    }

    return insert(entity);
}

std::unique_ptr<Entity> MemoryStorage::get_by_id(std::int64_t id) const
{
    auto it = entities_.find(id);
    if (it == entities_.end()) {
        return nullptr;
    }

    return std::make_unique<Entity>(clone_entity(it->second));
}

std::vector<std::unique_ptr<Entity>> MemoryStorage::query_by_field(std::size_t field_index, const FieldValue & value) const
{
    if (expected_field_count_ != 0 && field_index >= expected_field_count_) {
        throw std::out_of_range("Field index out of range");
    }

    std::vector<std::unique_ptr<Entity>> results;
    for (const auto & [_, entity] : entities_) {
        if (match_field_value(entity, field_index, value)) {
            results.push_back(std::make_unique<Entity>(clone_entity(entity)));
        }
    }

    return results;
}

std::size_t MemoryStorage::size() const
{
    return entities_.size();
}

bool MemoryStorage::empty() const
{
    return entities_.empty();
}

bool MemoryStorage::contains(std::int64_t id) const
{
    return entities_.find(id) != entities_.end();
}

MutationResult MemoryStorage::insert_batch(const std::vector<Entity> & entities)
{
    MutationResult result;
    for (const auto & entity : entities) {
        auto single = insert(entity);
        if (!single.success) {
            return single;
        }
        result.affected_count += single.affected_count;
    }
    result.success = true;
    return result;
}

MutationResult MemoryStorage::clear()
{
    const auto removed = entities_.size();
    entities_.clear();
    return make_success(removed);
}

Entity MemoryStorage::clone_entity(const Entity & entity) const
{
    Entity clone(entity.get_id(), entity.field_count());
    for (std::size_t i = 0; i < entity.field_count(); ++i) {
        clone.set_value(i, entity.get_value(i));
    }

    return clone;
}

bool MemoryStorage::match_field_value(const Entity & entity, std::size_t field_index, const FieldValue & value) const
{
    if (field_index >= entity.field_count()) {
        throw std::out_of_range("Field index out of range");
    }

    return entity.get_value(field_index) == value;
}

} // namespace dreamdb

