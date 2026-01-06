#include "dreamdb/catalog/catalog_database_entry.h"

#include <stdexcept>

namespace dreamdb
{

CatalogDatabaseEntry::CatalogDatabaseEntry(const std::string & database_name)
    : CatalogEntry(CatalogEntryType::CATALOG_ENTRY_TYPE_DATABASE, database_name)
{
}

// ========== 集合查询接口 ==========

const CatalogCollectionEntry * CatalogDatabaseEntry::get_collection_entry(std::size_t collection_index) const noexcept
{
    if (collection_index >= collection_entries_.size()) {
        return nullptr;
    }
    return collection_entries_[collection_index].get();
}

const CatalogCollectionEntry * CatalogDatabaseEntry::get_collection_entry(const std::string & collection_name) const noexcept
{
    auto it = collection_index_map_.find(collection_name);
    if (it == collection_index_map_.end()) {
        return nullptr;
    }
    return get_collection_entry(it->second);
}

std::vector<std::string> CatalogDatabaseEntry::get_collection_names() const
{
    std::vector<std::string> names;
    names.reserve(collection_entries_.size());
    for (const auto& entry : collection_entries_) {
        names.push_back(entry->name());
    }
    return names;
}

bool CatalogDatabaseEntry::has_collection(const std::string & collection_name) const noexcept
{
    return collection_index_map_.find(collection_name) != collection_index_map_.end();
}

// ========== 集合管理接口 ==========

void CatalogDatabaseEntry::add_collection(std::unique_ptr<CatalogCollectionEntry> collection_entry)
{
    const std::string & collection_name = collection_entry->name();

    // 检查集合名称是否已存在
    if (collection_index_map_.find(collection_name) != collection_index_map_.end()) {
        throw std::runtime_error("Collection name already exists");
    }

    // 添加集合条目
    collection_entries_.push_back(std::move(collection_entry));
    collection_index_map_[collection_name] = collection_entries_.size() - 1;
}

bool CatalogDatabaseEntry::remove_collection(const std::string & collection_name)
{
    auto it = collection_index_map_.find(collection_name);
    if (it == collection_index_map_.end()) {
        return false;
    }
    collection_entries_.erase(collection_entries_.begin() + it->second);
    collection_index_map_.erase(it);
    return true;
}

bool CatalogDatabaseEntry::rename_collection(const std::string & collection_name, const std::string & new_collection_name)
{
    auto it = collection_index_map_.find(collection_name);
    if (it == collection_index_map_.end()) {
        return false;
    }
    collection_index_map_[new_collection_name] = it->second;
    collection_index_map_.erase(it);
    return true;
}

} // namespace dreamdb
