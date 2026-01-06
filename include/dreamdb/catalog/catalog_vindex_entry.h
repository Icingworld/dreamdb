#pragma once

#include <string>
#include <cstddef>
#include <unordered_map>
#include <optional>

#include "dreamdb/catalog/catalog_entry.h"
#include "dreamdb/common/type.h"

namespace dreamdb
{

/**
 * @brief 向量索引条目
 * @details 存储向量索引的元数据信息
 */
class CatalogVIndexEntry : public CatalogEntry
{
public:
    /**
     * @brief 构造函数
     * @param vindex_name 向量索引名称
     * @param column_index 列索引（向量索引只作用于单个列）
     * @param vindex_type 向量索引类型（FLAT、IVF_FLAT、HNSW）
     * @param metric_type 距离度量类型（L2、IP、COSINE），可选
     */
    CatalogVIndexEntry(
        const std::string& vindex_name,
        std::size_t column_index,
        VIndexType vindex_type,
        const std::optional<MetricType>& metric_type = std::nullopt
    );

    CatalogVIndexEntry(const CatalogVIndexEntry&) = default;
    CatalogVIndexEntry(CatalogVIndexEntry&&) noexcept = default;
    CatalogVIndexEntry& operator=(const CatalogVIndexEntry&) = default;
    CatalogVIndexEntry& operator=(CatalogVIndexEntry&&) noexcept = default;
    
    virtual ~CatalogVIndexEntry() noexcept = default;

public:
    /**
     * @brief 获取列索引
     * @return 列索引
     */
    std::size_t column_index() const noexcept;

    /**
     * @brief 获取向量索引类型
     * @return 向量索引类型
     */
    VIndexType vindex_type() const noexcept;

    /**
     * @brief 获取距离度量类型
     * @return 距离度量类型，如果未设置则返回 std::nullopt
     */
    const std::optional<MetricType>& metric_type() const noexcept;

    /**
     * @brief 设置距离度量类型
     * @param metric_type 距离度量类型
     */
    void set_metric_type(const std::optional<MetricType>& metric_type) noexcept;

    /**
     * @brief 设置配置选项
     * @param key 配置键
     * @param value 配置值（字符串形式）
     */
    void set_option(const std::string& key, const std::string& value);

    /**
     * @brief 获取配置选项
     * @param key 配置键
     * @return 配置值，如果不存在则返回 std::nullopt
     */
    std::optional<std::string> get_option(const std::string& key) const;

    /**
     * @brief 获取所有配置选项
     * @return 配置选项映射
     */
    const std::unordered_map<std::string, std::string>& get_all_options() const noexcept;

private:
    std::size_t column_index_;                                    // 列索引
    VIndexType vindex_type_;                                      // 向量索引类型
    std::optional<MetricType> metric_type_;                       // 距离度量类型
    std::unordered_map<std::string, std::string> options_;       // 其他配置选项（如 nlist, m, ef_construction 等）
};

} // namespace dreamdb
