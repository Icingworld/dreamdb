#include "dreamdb/schema/database_manager.h"

#include "dreamdb/schema/database.h"
#include "dreamdb/catalog/catalog_database_entry.h"

namespace dreamdb
{

DatabaseManager::~DatabaseManager() = default;

std::size_t DatabaseManager::create_database(const std::string & name)
{
    // 确保 Catalog 已初始化
    if (!catalog_) {
        catalog_ = std::make_unique<Catalog>();
    }

    // 检查数据库是否已存在
    const CatalogDatabaseEntry * existing_entry = catalog_->get_database_entry(name);
    if (existing_entry) {
        // 如果数据库已存在于 Catalog，检查是否也在 databases_ 中
        std::size_t database_id = existing_entry->database_id_;
        if (databases_.find(database_id) == databases_.end()) {
            // 如果不在 databases_ 中，创建 Database 对象
            auto db = std::make_unique<Database>(name);
            databases_.emplace(database_id, std::move(db));
        }
        return database_id;
    }

    // 创建数据库条目并添加到 Catalog
    auto database_entry = std::make_unique<CatalogDatabaseEntry>(name);
    std::size_t database_id = database_entry->database_id_;
    catalog_->add_database(std::move(database_entry));

    // 创建 Database 对象
    auto db = std::make_unique<Database>(name);
    databases_.emplace(database_id, std::move(db));

    // 如果没有当前数据库，设置为新创建的数据库
    if (current_database_id_ == 0) {
        current_database_id_ = database_id;
    }

    return database_id;
}

bool DatabaseManager::drop_database(std::size_t id)
{
    auto it = databases_.find(id);
    if (it == databases_.end()) {
        return false;
    }

    // 从 Catalog 获取数据库名称并删除
    if (catalog_) {
        const CatalogDatabaseEntry * database_entry = catalog_->get_database_entry(id);
        if (database_entry) {
            catalog_->remove_database(database_entry->database_name_);
        }
    }

    // 如果删除的是当前数据库，清空当前数据库引用
    if (current_database_id_ == id) {
        current_database_id_ = 0;
    }

    // 删除该数据库对象
    databases_.erase(it);
    return true;
}

void DatabaseManager::set_current_database(std::size_t id)
{
    current_database_id_ = id;
}

Database * DatabaseManager::get_current_database()
{
    if (current_database_id_ == 0) {
        return nullptr;
    }

    auto it = databases_.find(current_database_id_);
    if (it == databases_.end()) {
        // 没有选择数据库或已经被删除
        return nullptr;
    }

    return it->second.get();
}

bool DatabaseManager::has_database(std::size_t id) const
{
    return databases_.find(id) != databases_.end();
}

std::vector<std::size_t> DatabaseManager::get_databases() const
{
    std::vector<std::size_t> database_ids;
    database_ids.reserve(databases_.size());
    for (const auto & [id, _] : databases_) {
        database_ids.push_back(id);
    }
    return database_ids;
}

const Catalog & DatabaseManager::get_catalog() const noexcept
{
    // 如果 Catalog 未初始化，创建一个新的（注意：这违反了 const 语义，但为了兼容性）
    if (!catalog_) {
        const_cast<DatabaseManager *>(this)->catalog_ = std::make_unique<Catalog>();
    }
    return *catalog_;
}

Catalog & DatabaseManager::get_catalog() noexcept
{
    // 如果 Catalog 未初始化，创建一个新的
    if (!catalog_) {
        catalog_ = std::make_unique<Catalog>();
    }
    return *catalog_;
}

} // namespace dreamdb
