#include "dreamdb/schema/database.h"

namespace dreamdb
{

Database::Database(const std::string & name)
    : name_(name)
    , collection_manager_(std::make_unique<CollectionManager>())
{
}

const std::string & Database::get_name() const
{
    return name_;
}

Collection * Database::create_collection(const std::string & name, const std::vector<Field> & schema)
{
    return collection_manager_->create_collection(name, schema);
}

bool Database::drop_collection(const std::string & name)
{
    return collection_manager_->drop_collection(name);
}

Collection * Database::get_collection(const std::string & name)
{
    return collection_manager_->get_collection(name);
}

} // namespace dreamdb
