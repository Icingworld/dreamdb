#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "dreamdb/catalog/catalog_entry.h"
#include "dreamdb/catalog/catalog_collection_entry.h"

namespace dreamdb
{

class CatalogDatabaseEntry : public CatalogEntry
{
public:
    CatalogDatabaseEntry(const std::string & database_name);
    virtual ~CatalogDatabaseEntry() noexcept = default;

public:
    // ========== 集合查询接口 ==========
    const CatalogCollectionEntry * get_collection_entry(std::size_t collection_index) const noexcept;
    const CatalogCollectionEntry * get_collection_entry(const std::string & collection_name) const noexcept;

    /**
     * @brief 获取所有集合名称
     * @return 集合名称列表
     */
    std::vector<std::string> get_collection_names() const;

    /**
     * @brief 检查集合是否存在
     * @param collection_name 集合名称
     * @return 是否存在
     */
    bool has_collection(const std::string & collection_name) const noexcept;

public:
    // ========== 集合管理接口 ==========
    void add_collection(std::unique_ptr<CatalogCollectionEntry> collection_entry);

    bool remove_collection(const std::string & collection_name);

    bool rename_collection(const std::string & collection_name, const std::string & new_collection_name);

public:
    std::size_t database_id_; // 数据库 ID
    std::string database_name_; // 数据库名称
    std::vector<std::unique_ptr<CatalogCollectionEntry>> collection_entries_; // 集合条目列表
    std::unordered_map<std::string, std::size_t> collection_index_map_; // 集合索引映射 Map: 集合名称 -> 集合索引位置
};

} // namespace dreamdb
