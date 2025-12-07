#include "dreamdb/schema/database.h"

namespace dreamdb
{

Database::Database(const std::string & name, CollectionManager::StorageFactory storage_factory)
    : name_(name)
    , collection_manager_(std::make_unique<CollectionManager>(storage_factory))
{
}

const std::string & Database::get_name() const
{
    return name_;
}

CollectionManager & Database::get_collection_manager()
{
    return *collection_manager_;
}

const CollectionManager & Database::get_collection_manager() const
{
    return *collection_manager_;
}

} // namespace dreamdb
