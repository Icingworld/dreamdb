#include "dreamdb/schema/collection.h"

namespace dreamdb
{

Collection::Collection(
    const std::string & name, 
    const std::vector<Field> & schema
)
    : name(name)
    , schema(schema)
    , next_id(1)
{
}

void Collection::set_name(const std::string & name)
{
    this->name = name;
}

void Collection::set_schema(const std::vector<Field> & schema)
{
    this->schema = schema;
}

const std::string & Collection::get_name() const
{
    return name;
}

const std::vector<Field> & Collection::get_schema() const
{
    return schema;
}

Entity Collection::create_entity()
{
    // 预分配字段数量
    Entity entity(next_id++, schema.size());

    return entity;
}

MutationResult Collection::insert(const Entity & entity)
{
    return MutationResult();
}

MutationResult Collection::remove(const std::string & key, const std::string & value)
{
    return MutationResult();
}

MutationResult Collection::upsert(const std::string & key, const std::string & value, const Entity & entity)
{
    return MutationResult();
}

} // namespace dreamdb
