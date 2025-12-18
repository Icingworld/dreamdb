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
    , next_id_(1)
    , segment_manager_(std::move(segment_manager))
    , active_segment_(segment_manager_->create_segment())
{
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

MutationResult Collection::remove(std::int64_t id)
{
    return active_segment_->remove_by_id(id);
}

MutationResult Collection::update(std::int64_t id, std::vector<std::pair<std::size_t, FieldValue>> fields)
{
    return active_segment_->update_by_id(id, fields);
}

std::vector<std::unique_ptr<Entity>> Collection::query(const Query & query) const
{
    return active_segment_->query(query);
}

} // namespace dreamdb
