#pragma once

#include <string>
#include <vector>
#include <cstddef>

#include "dreamdb/catalog/catalog_entry.h"
#include "dreamdb/common/type.h"

namespace dreamdb
{

/**
 * @brief 索引条目
 * @details 存储标量索引的元数据信息
 */
class CatalogIndexEntry : public CatalogEntry
{
public:
    /**
     * @brief 构造函数
     * @param index_name 索引名称
     * @param index_type 索引类型（BTREE 或 HASH）
     * @param is_unique 是否唯一索引
     * @param field_indexes 字段索引列表
     */
    CatalogIndexEntry(
        const std::string& index_name,
        IndexType index_type,
        bool is_unique,
        const std::vector<std::size_t>& field_indexes
    );

    CatalogIndexEntry(const CatalogIndexEntry&) = default;
    CatalogIndexEntry(CatalogIndexEntry&&) noexcept = default;
    CatalogIndexEntry& operator=(const CatalogIndexEntry&) = default;
    CatalogIndexEntry& operator=(CatalogIndexEntry&&) noexcept = default;
    
    virtual ~CatalogIndexEntry() noexcept = default;

public:
    /**
     * @brief 获取索引类型
     * @return 索引类型
     */
    IndexType index_type() const noexcept;

    /**
     * @brief 是否唯一索引
     * @return 是否唯一
     */
    bool is_unique() const noexcept;

    /**
     * @brief 获取字段索引列表
     * @return 字段索引列表
     */
    const std::vector<std::size_t>& field_indexes() const noexcept;

    /**
     * @brief 判断是否包含指定字段索引
     * @param field_index 字段索引
     * @return 是否包含
     */
    bool has_field_index(std::size_t field_index) const noexcept;

private:
    IndexType index_type_;                    // 索引类型
    bool is_unique_;                          // 是否唯一
    std::vector<std::size_t> field_indexes_;  // 字段索引列表
};

} // namespace dreamdb
