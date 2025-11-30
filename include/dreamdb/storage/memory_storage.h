#pragma once

#include <unordered_map>

#include "dreamdb/storage/storage_base.h"

namespace dreamdb
{

/**
 * @brief 简单的内存存储实现
 * @details 使用 unordered_map 存储 Entity，主要用于测试或原型阶段
 */
class MemoryStorage final : public StorageBase
{
public:
    explicit MemoryStorage(std::size_t field_count = 0);

    ~MemoryStorage() override = default;

public:
    MutationResult insert(const Entity & entity) override;

    MutationResult remove_by_id(std::int64_t id) override;

    MutationResult remove_by_field(std::size_t field_index, const FieldValue & value) override;

    MutationResult update_by_id(std::int64_t id, const Entity & entity) override;

    MutationResult upsert_by_field(std::size_t field_index, const FieldValue & value, const Entity & entity) override;

    std::unique_ptr<Entity> get_by_id(std::int64_t id) const override;

    std::vector<std::unique_ptr<Entity>> query_by_field(std::size_t field_index, const FieldValue & value) const override;

    std::size_t size() const override;

    bool empty() const override;

    bool contains(std::int64_t id) const override;

    MutationResult insert_batch(const std::vector<Entity> & entities) override;

    MutationResult clear() override;

private:
    Entity clone_entity(const Entity & entity) const;

    bool match_field_value(const Entity & entity, std::size_t field_index, const FieldValue & value) const;

private:
    std::unordered_map<std::int64_t, Entity> entities_;
    std::size_t expected_field_count_;
};

} // namespace dreamdb

