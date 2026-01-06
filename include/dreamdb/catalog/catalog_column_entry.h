#pragma once

#include "dreamdb/catalog/catalog_entry.h"
#include "dreamdb/catalog/logical_type.h"

namespace dreamdb
{

/**
 * @brief 列条目
 */
class CatalogColumnEntry : public CatalogEntry
{
public:
    CatalogColumnEntry(const std::string & column_name, const LogicalType & logical_type, std::size_t column_index);

    virtual ~CatalogColumnEntry() noexcept = default;

public:
    const LogicalType & logical_type() const noexcept;
    std::size_t column_index() const noexcept;

private:
    LogicalType logical_type_;       // 逻辑类型
    std::size_t column_index_;       // 列索引位置
};

} // namespace dreamdb
