#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <optional>

#include "dreamdb/catalog/catalog_database_entry.h"
#include "dreamdb/catalog/catalog_collection_entry.h"
#include "dreamdb/catalog/catalog_column_entry.h"

namespace dreamdb
{

/**
 * @brief 列引用信息
 */
struct ColumnReference
{
    const CatalogDatabaseEntry* database;       // 数据库条目
    const CatalogCollectionEntry* collection;   // 集合条目
    const CatalogColumnEntry* column;           // 列条目
    std::size_t column_index;                   // 列索引
};

class Catalog
{
public:
    Catalog() = default;

    ~Catalog() noexcept = default;

public:
    // ========== 数据库查询接口 ==========
    const CatalogDatabaseEntry * get_database_entry(std::size_t database_index) const noexcept;

    const CatalogDatabaseEntry * get_database_entry(const std::string & database_name) const noexcept;

    /**
     * @brief 获取所有数据库名称
     * @return 数据库名称列表
     */
    std::vector<std::string> get_database_names() const;

    /**
     * @brief 检查数据库是否存在
     * @param database_name 数据库名称
     * @return 是否存在
     */
    bool has_database(const std::string & database_name) const noexcept;

    // ========== 集合便捷查询接口 ==========
    /**
     * @brief 获取集合条目（便捷接口）
     * @param database_name 数据库名称
     * @param collection_name 集合名称
     * @return 集合条目指针，不存在返回 nullptr
     */
    const CatalogCollectionEntry * get_collection_entry(
        const std::string & database_name,
        const std::string & collection_name
    ) const noexcept;

    /**
     * @brief 检查集合是否存在
     * @param database_name 数据库名称
     * @param collection_name 集合名称
     * @return 是否存在
     */
    bool has_collection(const std::string & database_name, const std::string & collection_name) const noexcept;

    // ========== 列便捷查询接口 ==========
    /**
     * @brief 获取列条目（便捷接口）
     * @param database_name 数据库名称
     * @param collection_name 集合名称
     * @param column_name 列名称
     * @return 列条目指针，不存在返回 nullptr
     */
    const CatalogColumnEntry * get_column_entry(
        const std::string & database_name,
        const std::string & collection_name,
        const std::string & column_name
    ) const noexcept;

    /**
     * @brief 检查列是否存在
     * @param database_name 数据库名称
     * @param collection_name 集合名称
     * @param column_name 列名称
     * @return 是否存在
     */
    bool has_column(
        const std::string & database_name,
        const std::string & collection_name,
        const std::string & column_name
    ) const noexcept;

    // ========== 列引用解析接口 ==========
    /**
     * @brief 解析列引用
     * @param database_name 数据库名称（可选）
     * @param collection_name 集合名称（可选）
     * @param column_name 列名称（必需）
     * @param current_database 当前数据库名称（用于解析未限定的名称）
     * @param current_collection 当前集合名称（可选，用于解析未限定的名称）
     * @return 列引用信息，如果解析失败返回 std::nullopt
     */
    std::optional<ColumnReference> resolve_column(
        const std::optional<std::string> & database_name,
        const std::optional<std::string> & collection_name,
        const std::string & column_name,
        const std::string & current_database,
        const std::optional<std::string> & current_collection = std::nullopt
    ) const noexcept;

public:
    // ========== 数据库管理接口 ==========
    void add_database(std::unique_ptr<CatalogDatabaseEntry> database_entry);

    bool remove_database(const std::string & database_name);

    bool rename_database(const std::string & database_name, const std::string & new_database_name);

private:
    // 数据库条目存储
    std::vector<std::unique_ptr<CatalogDatabaseEntry>> database_entries_; // 数据库条目列表
    std::unordered_map<std::string, std::size_t> database_index_map_; // 数据库索引映射 Map: 数据库名称 -> 数据库索引位置

    // TODO: 函数存储
};

} // namespace dreamdb
