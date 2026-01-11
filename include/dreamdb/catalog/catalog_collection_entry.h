#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

#include "dreamdb/catalog/catalog_entry.h"
#include "dreamdb/catalog/catalog_column_entry.h"
#include "dreamdb/catalog/catalog_index_entry.h"
#include "dreamdb/catalog/catalog_vindex_entry.h"

namespace dreamdb
{

/**
 * @brief 集合条目
 */
class CatalogCollectionEntry : public CatalogEntry
{
public:
    CatalogCollectionEntry(const std::string & collection_name);

    virtual ~CatalogCollectionEntry() noexcept = default;

public:
    // ========== 列条目查询接口 ==========
    const CatalogColumnEntry * get_column_entry(std::size_t column_index) const noexcept;

    const CatalogColumnEntry * get_column_entry(const std::string & column_name) const noexcept;

    /**
     * @brief 获取所有列名称
     * @return 列名称列表
     */
    std::vector<std::string> get_column_names() const;

    /**
     * @brief 检查列是否存在
     * @param column_name 列名称
     * @return 是否存在
     */
    bool has_column(const std::string & column_name) const noexcept;

    // ========== 索引条目查询接口 ==========
    const CatalogIndexEntry * get_index_entry(const std::string & index_name) const noexcept;

    /**
     * @brief 根据字段索引查找相关索引
     * @param field_index 字段索引
     * @return 相关索引条目列表
     */
    std::vector<const CatalogIndexEntry*> find_indexes_by_field(std::size_t field_index) const;

    /**
     * @brief 获取所有索引条目
     * @return 所有索引条目列表
     */
    std::vector<const CatalogIndexEntry*> get_all_indexes() const;

    // ========== 向量索引条目查询接口 ==========
    const CatalogVIndexEntry * get_vindex_entry(const std::string & vindex_name) const noexcept;

    /**
     * @brief 根据列索引查找向量索引
     * @param column_index 列索引
     * @return 向量索引条目，如果不存在返回 nullptr
     */
    const CatalogVIndexEntry * find_vindex_by_column(std::size_t column_index) const;

    /**
     * @brief 获取所有向量索引条目
     * @return 所有向量索引条目列表
     */
    std::vector<const CatalogVIndexEntry*> get_all_vindexes() const;

public:
    // ========== 列条目管理接口 ==========
    /**
     * @brief 添加列条目
     * @param column_entry 列条目
     * @details 仅在构建期间使用
     */
    void add_column(std::unique_ptr<CatalogColumnEntry> column_entry);

    bool remove_column(const std::string & column_name);

    bool modify_column(const std::string & column_name, const CatalogColumnEntry & new_column_entry);

    // ========== 索引条目管理接口 ==========
    /**
     * @brief 添加索引条目
     * @param index_entry 索引条目
     */
    void add_index(std::unique_ptr<CatalogIndexEntry> index_entry);

    /**
     * @brief 删除索引条目
     * @param index_name 索引名称
     * @return 是否删除成功
     */
    bool remove_index(const std::string & index_name);

    // ========== 向量索引条目管理接口 ==========
    /**
     * @brief 添加向量索引条目
     * @param vindex_entry 向量索引条目
     */
    void add_vindex(std::unique_ptr<CatalogVIndexEntry> vindex_entry);

    /**
     * @brief 删除向量索引条目
     * @param vindex_name 向量索引名称
     * @return 是否删除成功
     */
    bool remove_vindex(const std::string & vindex_name);

public:
    std::size_t collection_id_; // 集合 ID
    std::string collection_name_; // 集合名称

    // 列条目存储
    std::vector<std::unique_ptr<CatalogColumnEntry>> column_entries_; // 列条目列表
    std::unordered_map<std::string, std::size_t> column_index_map_; // 列索引映射 Map: 列名称 -> 列索引位置

    // 索引条目存储
    std::unordered_map<std::string, std::unique_ptr<CatalogIndexEntry>> index_entries_; // 索引条目映射
    std::unordered_map<std::size_t, std::vector<std::string>> field_to_indexes_; // 字段索引到索引名的映射

    // 向量索引条目存储
    std::unordered_map<std::string, std::unique_ptr<CatalogVIndexEntry>> vindex_entries_; // 向量索引条目映射
    std::unordered_map<std::size_t, std::string> column_to_vindex_; // 列索引到向量索引名的映射
};

} // namespace dreamdb
