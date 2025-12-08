#include "dreamdb/schema/database_manager.h"
#include "dreamdb/storage/memory_storage.h"

namespace dreamdb
{

DatabaseManager::DatabaseManager(CollectionManager::StorageFactory default_factory)
    : current_database_("")
    , databases_()
    , default_factory_(default_factory ? default_factory : 
        [](std::size_t field_count) {
            return std::make_unique<MemoryStorage>(field_count);
        })
{
}

Database * DatabaseManager::create_database(const std::string & name, CollectionManager::StorageFactory storage_factory)
{
    if (has_database(name)) {
        return nullptr;
    }

    // 如果没有提供 factory，使用默认的
    auto factory = storage_factory ? storage_factory : default_factory_;

    auto database = std::make_unique<Database>(name, factory);
    auto * database_ptr = database.get();
    databases_[name] = std::move(database);
    return database_ptr;
}

void DatabaseManager::set_default_storage_factory(CollectionManager::StorageFactory factory)
{
    default_factory_ = factory ? factory : 
        [](std::size_t field_count) {
            return std::make_unique<MemoryStorage>(field_count);
        };
}

bool DatabaseManager::drop_database(const std::string & name)
{
    auto it = databases_.find(name);
    if (it == databases_.end()) {
        return false;
    }

    // TODO: 清除数据库内容

    // 如果删除的是当前数据库，清空当前数据库引用
    if (current_database_ == name) {
        current_database_.clear();
    }

    // 删除该数据库对象
    databases_.erase(it);
    return true;
}

Database * DatabaseManager::get_database(const std::string & name)
{
    auto it = databases_.find(name);
    if (it == databases_.end()) {
        return nullptr;
    }
    return it->second.get();
}

bool DatabaseManager::has_database(const std::string & name) const
{
    return databases_.find(name) != databases_.end();
}

void DatabaseManager::set_current_database(const std::string & name)
{
    current_database_ = name;
}

Database * DatabaseManager::get_current_database()
{
    if (current_database_.empty()) {
        return nullptr;
    }

    auto it = databases_.find(current_database_);
    if (it == databases_.end()) {
        // 没有选择数据库或已经被删除
        return nullptr;
    }

    return it->second.get();
}

const Database * DatabaseManager::get_current_database() const
{
    if (current_database_.empty()) {
        return nullptr;
    }

    auto it = databases_.find(current_database_);
    if (it == databases_.end()) {
        // 没有选择数据库或已经被删除
        return nullptr;
    }

    return it->second.get();
}

std::vector<std::string> DatabaseManager::list_databases() const
{
    std::vector<std::string> names;
    names.reserve(databases_.size());
    for (const auto & [name, _] : databases_) {
        names.push_back(name);
    }
    return names;
}

} // namespace dreamdb
