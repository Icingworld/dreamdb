#include "dreamdb/schema/database_manager.h"

#include "dreamdb/schema/database.h"

namespace dreamdb
{

DatabaseManager::~DatabaseManager() = default;

Database * DatabaseManager::create_database(const std::string & name)
{
    if (databases_.find(name) != databases_.end()) {
        return nullptr;
    }

    auto db = std::make_unique<Database>(name);
    auto * db_ptr = db.get();
    databases_.emplace(name, std::move(db));

    if (current_database_.empty()) {
        current_database_ = name;
    }

    return db_ptr;
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

bool DatabaseManager::has_database(const std::string & name) const
{
    return databases_.find(name) != databases_.end();
}

std::vector<std::string> DatabaseManager::get_databases() const
{
    std::vector<std::string> databases;
    for (const auto & [name, _] : databases_) {
        databases.push_back(name);
    }
    return databases;
}

} // namespace dreamdb
