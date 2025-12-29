#include "dreamdb/schema/collection.h"

#include "dreamdb/storage/segment_manager.h"
#include "dreamdb/storage/segment.h"

namespace dreamdb
{

Collection::Collection(
    const std::string & name,
    const std::vector<Field> & schema,
    std::unique_ptr<SegmentManager> segment_manager
)
    : name_(name)
    , schema_(schema)
    , field_index_map_()
    , next_id_(1)
    , segment_manager_(std::move(segment_manager))
    , active_segment_(segment_manager_->create_segment())
{
    // 初始化字段映射表
    for (std::size_t i = 0; i < schema.size(); ++i) {
        field_index_map_[schema[i].get_name()] = i;
    }
}

Collection::~Collection() = default;

void Collection::set_name(const std::string & name)
{
    name_ = name;
}

const std::string & Collection::get_name() const
{
    return name_;
}

const std::vector<Field> & Collection::get_schema() const
{
    return schema_;
}

std::optional<std::size_t> Collection::get_field_index(const std::string & name) const
{
    auto it = field_index_map_.find(name);
    if (it != field_index_map_.end()) {
        return it->second;
    }

    return std::nullopt;
}

Entity Collection::create_entity()
{
    // 预分配字段数量
    Entity entity(next_id_++, schema_.size());

    return entity;
}

MutationResult Collection::insert(const Entity & entity)
{
    // TODO: 检查是否需要 Seal 段
    return active_segment_->insert(entity);
}

MutationResult Collection::remove(std::size_t id)
{
    return active_segment_->remove_by_id(id);
}

MutationResult Collection::update(std::size_t id, std::vector<std::pair<std::size_t, FieldValue>> fields)
{
    return active_segment_->update_by_id(id, fields);
}

std::vector<std::unique_ptr<Entity>> Collection::query(const Query & query) const
{
    return active_segment_->query(query, this);
}

} // namespace dreamdb
