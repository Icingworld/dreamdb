#include "dreamdb/catalog/catalog_collection_entry.h"

#include <stdexcept>
#include <algorithm>

namespace dreamdb
{

CatalogCollectionEntry::CatalogCollectionEntry(const std::string & collection_name)
    : CatalogEntry(CatalogEntryType::CATALOG_ENTRY_TYPE_COLLECTION, collection_name)
{
}

const CatalogColumnEntry * CatalogCollectionEntry::get_column_entry(std::size_t column_index) const noexcept
{
    if (column_index >= column_entries_.size()) {
        return nullptr;
    }
    return column_entries_[column_index].get();
}

const CatalogColumnEntry * CatalogCollectionEntry::get_column_entry(const std::string & column_name) const noexcept
{
    auto it = column_index_map_.find(column_name);
    if (it == column_index_map_.end()) {
        return nullptr;
    }
    return get_column_entry(it->second);
}

std::vector<std::string> CatalogCollectionEntry::get_column_names() const
{
    std::vector<std::string> names;
    names.reserve(column_entries_.size());
    for (const auto& entry : column_entries_) {
        names.push_back(entry->name());
    }
    return names;
}

bool CatalogCollectionEntry::has_column(const std::string & column_name) const noexcept
{
    return column_index_map_.find(column_name) != column_index_map_.end();
}

void CatalogCollectionEntry::add_column(std::unique_ptr<CatalogColumnEntry> column_entry)
{
    const std::string & column_name = column_entry->name();

    // 检查列名称是否已存在
    if (column_index_map_.find(column_name) != column_index_map_.end()) {
        throw std::runtime_error("Column name already exists");
    }

    // 添加列条目
    column_entries_.push_back(std::move(column_entry));
    column_index_map_[column_name] = column_entries_.size() - 1;
}

bool CatalogCollectionEntry::remove_column(const std::string & column_name)
{
    auto it = column_index_map_.find(column_name);
    if (it == column_index_map_.end()) {
        return false;
    }
    column_entries_.erase(column_entries_.begin() + it->second);
    column_index_map_.erase(it);
    return true;
}

bool CatalogCollectionEntry::modify_column(const std::string & column_name, const CatalogColumnEntry & new_column_entry)
{
    auto it = column_index_map_.find(column_name);
    if (it == column_index_map_.end()) {
        return false;
    }
    column_entries_[it->second] = std::make_unique<CatalogColumnEntry>(new_column_entry);
    return true;
}

// ========== 索引条目查询接口 ==========

const CatalogIndexEntry * CatalogCollectionEntry::get_index_entry(const std::string & index_name) const noexcept
{
    auto it = index_entries_.find(index_name);
    if (it == index_entries_.end()) {
        return nullptr;
    }
    return it->second.get();
}

std::vector<const CatalogIndexEntry*> CatalogCollectionEntry::find_indexes_by_field(std::size_t field_index) const
{
    std::vector<const CatalogIndexEntry*> result;
    auto it = field_to_indexes_.find(field_index);
    if (it != field_to_indexes_.end()) {
        for (const auto& index_name : it->second) {
            auto index_it = index_entries_.find(index_name);
            if (index_it != index_entries_.end()) {
                result.push_back(index_it->second.get());
            }
        }
    }
    return result;
}

std::vector<const CatalogIndexEntry*> CatalogCollectionEntry::get_all_indexes() const
{
    std::vector<const CatalogIndexEntry*> result;
    result.reserve(index_entries_.size());
    for (const auto& pair : index_entries_) {
        result.push_back(pair.second.get());
    }
    return result;
}

// ========== 向量索引条目查询接口 ==========

const CatalogVIndexEntry * CatalogCollectionEntry::get_vindex_entry(const std::string & vindex_name) const noexcept
{
    auto it = vindex_entries_.find(vindex_name);
    if (it == vindex_entries_.end()) {
        return nullptr;
    }
    return it->second.get();
}

const CatalogVIndexEntry * CatalogCollectionEntry::find_vindex_by_column(std::size_t column_index) const
{
    auto it = column_to_vindex_.find(column_index);
    if (it == column_to_vindex_.end()) {
        return nullptr;
    }
    return get_vindex_entry(it->second);
}

std::vector<const CatalogVIndexEntry*> CatalogCollectionEntry::get_all_vindexes() const
{
    std::vector<const CatalogVIndexEntry*> result;
    result.reserve(vindex_entries_.size());
    for (const auto& pair : vindex_entries_) {
        result.push_back(pair.second.get());
    }
    return result;
}

// ========== 索引条目管理接口 ==========

void CatalogCollectionEntry::add_index(std::unique_ptr<CatalogIndexEntry> index_entry)
{
    const std::string& index_name = index_entry->name();

    // 检查索引名称是否已存在
    if (index_entries_.find(index_name) != index_entries_.end()) {
        throw std::runtime_error("Index name already exists");
    }

    // 添加索引条目
    index_entries_[index_name] = std::move(index_entry);

    // 更新字段到索引的映射
    const auto& field_indexes = index_entries_[index_name]->field_indexes();
    for (std::size_t field_index : field_indexes) {
        field_to_indexes_[field_index].push_back(index_name);
    }
}

bool CatalogCollectionEntry::remove_index(const std::string & index_name)
{
    auto it = index_entries_.find(index_name);
    if (it == index_entries_.end()) {
        return false;
    }

    // 从字段到索引的映射中移除
    const auto& field_indexes = it->second->field_indexes();
    for (std::size_t field_index : field_indexes) {
        auto& index_list = field_to_indexes_[field_index];
        index_list.erase(
            std::remove(index_list.begin(), index_list.end(), index_name),
            index_list.end()
        );
        if (index_list.empty()) {
            field_to_indexes_.erase(field_index);
        }
    }

    // 删除索引条目
    index_entries_.erase(it);
    return true;
}

// ========== 向量索引条目管理接口 ==========

void CatalogCollectionEntry::add_vindex(std::unique_ptr<CatalogVIndexEntry> vindex_entry)
{
    const std::string& vindex_name = vindex_entry->name();
    std::size_t column_index = vindex_entry->column_index();

    // 检查向量索引名称是否已存在
    if (vindex_entries_.find(vindex_name) != vindex_entries_.end()) {
        throw std::runtime_error("Vector index name already exists");
    }

    // 检查该列是否已有向量索引
    if (column_to_vindex_.find(column_index) != column_to_vindex_.end()) {
        throw std::runtime_error("Column already has a vector index");
    }

    // 添加向量索引条目
    vindex_entries_[vindex_name] = std::move(vindex_entry);

    // 更新列到向量索引的映射
    column_to_vindex_[column_index] = vindex_name;
}

bool CatalogCollectionEntry::remove_vindex(const std::string & vindex_name)
{
    auto it = vindex_entries_.find(vindex_name);
    if (it == vindex_entries_.end()) {
        return false;
    }

    // 从列到向量索引的映射中移除
    std::size_t column_index = it->second->column_index();
    column_to_vindex_.erase(column_index);

    // 删除向量索引条目
    vindex_entries_.erase(it);
    return true;
}

} // namespace dreamdb
