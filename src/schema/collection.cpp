#include "dreamdb/schema/collection.h"

namespace dreamdb::schema
{

Collection::Collection(
    dreamdb::common::collection_id_t id,
    std::string name,
    dreamdb::common::database_id_t database_id,
    std::vector<ColumnDefinition> columns
)
    : id_(id)
    , name_(std::move(name))
    , database_id_(database_id)
    , columns_(std::move(columns))
{
}

dreamdb::common::collection_id_t Collection::id() const noexcept
{
    return id_;
}

const std::string & Collection::name() const noexcept
{
    return name_;
}

dreamdb::common::database_id_t Collection::database_id() const noexcept
{
    return database_id_;
}

const std::vector<ColumnDefinition> & Collection::columns() const noexcept
{
    return columns_;
}

} // namespace dreamdb::schema
