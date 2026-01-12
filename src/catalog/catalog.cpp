#include "dreamdb/catalog/catalog.h"

#include <stdexcept>
#include <algorithm>

namespace dreamdb
{

// ========== 数据库查询接口 ==========

const CatalogDatabaseEntry * Catalog::get_database_entry(std::size_t database_index) const noexcept
{
    if (database_index >= database_entries_.size()) {
        return nullptr;
    }
    return database_entries_[database_index].get();
}

const CatalogDatabaseEntry * Catalog::get_database_entry(const std::string & database_name) const noexcept
{
    auto it = database_index_map_.find(database_name);
    if (it == database_index_map_.end()) {
        return nullptr;
    }
    return get_database_entry(it->second);
}

std::vector<std::string> Catalog::get_database_names() const
{
    std::vector<std::string> names;
    names.reserve(database_entries_.size());
    for (const auto& entry : database_entries_) {
        names.push_back(entry->name());
    }
    return names;
}

bool Catalog::has_database(const std::string & database_name) const noexcept
{
    return database_index_map_.find(database_name) != database_index_map_.end();
}

// ========== 集合便捷查询接口 ==========

const CatalogCollectionEntry * Catalog::get_collection_entry(
    const std::string & database_name,
    const std::string & collection_name
) const noexcept
{
    const auto* database = get_database_entry(database_name);
    if (!database) {
        return nullptr;
    }
    return database->get_collection_entry(collection_name);
}

bool Catalog::has_collection(const std::string & database_name, const std::string & collection_name) const noexcept
{
    return get_collection_entry(database_name, collection_name) != nullptr;
}

// ========== 列便捷查询接口 ==========

const CatalogColumnEntry * Catalog::get_column_entry(
    const std::string & database_name,
    const std::string & collection_name,
    const std::string & column_name
) const noexcept
{
    const auto* collection = get_collection_entry(database_name, collection_name);
    if (!collection) {
        return nullptr;
    }
    return collection->get_column_entry(column_name);
}

bool Catalog::has_column(
    const std::string & database_name,
    const std::string & collection_name,
    const std::string & column_name
) const noexcept
{
    return get_column_entry(database_name, collection_name, column_name) != nullptr;
}

// ========== 列引用解析接口 ==========

std::optional<ColumnReference> Catalog::resolve_column(
    const std::optional<std::string> & database_name,
    const std::optional<std::string> & collection_name,
    const std::string & column_name,
    const std::string & current_database,
    const std::optional<std::string> & current_collection
) const noexcept
{
    // 确定数据库名称
    std::string resolved_database = database_name.has_value() ? database_name.value() : current_database;
    if (resolved_database.empty()) {
        return std::nullopt;
    }

    // 确定集合名称
    std::string resolved_collection;
    if (collection_name.has_value()) {
        resolved_collection = collection_name.value();
    } else if (current_collection.has_value()) {
        resolved_collection = current_collection.value();
    } else {
        // 如果没有指定集合名称，无法解析
        return std::nullopt;
    }

    // 查找列
    const auto* column = get_column_entry(resolved_database, resolved_collection, column_name);
    if (!column) {
        return std::nullopt;
    }

    // 获取数据库和集合条目
    const auto* database = get_database_entry(resolved_database);
    const auto* collection = get_collection_entry(resolved_database, resolved_collection);
    if (!database || !collection) {
        return std::nullopt;
    }

    return ColumnReference{
        database,
        collection,
        column,
        column->column_index()
    };
}

// ========== 数据库管理接口 ==========

void Catalog::add_database(std::unique_ptr<CatalogDatabaseEntry> database_entry)
{
    const std::string & database_name = database_entry->name();

    // 检查数据库名称是否已存在
    if (database_index_map_.find(database_name) != database_index_map_.end()) {
        throw std::runtime_error("Database name already exists");
    }

    // 添加数据库条目
    database_entries_.push_back(std::move(database_entry));
    database_index_map_[database_name] = database_entries_.size() - 1;
}

bool Catalog::remove_database(const std::string & database_name)
{
    auto it = database_index_map_.find(database_name);
    if (it == database_index_map_.end()) {
        return false;
    }
    database_entries_.erase(database_entries_.begin() + it->second);
    database_index_map_.erase(it);
    return true;
}

bool Catalog::rename_database(const std::string & database_name, const std::string & new_database_name)
{
    auto it = database_index_map_.find(database_name);
    if (it == database_index_map_.end()) {
        return false;
    }
    database_index_map_[new_database_name] = it->second;
    database_index_map_.erase(it);
    return true;
}

} // namespace dreamdb
