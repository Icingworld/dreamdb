#pragma once

#include <cstdint>
#include <string>

namespace dreamdb
{

/**
 * @brief 目录条目类型
 */
enum class CatalogEntryType : std::uint8_t
{
    CATALOG_ENTRY_TYPE_DATABASE,      // 数据库条目
    CATALOG_ENTRY_TYPE_COLLECTION,    // 集合条目
    CATALOG_ENTRY_TYPE_COLUMN,        // 列条目
    CATALOG_ENTRY_TYPE_INDEX,         // 索引条目
    CATALOG_ENTRY_TYPE_VINDEX,        // 向量索引条目
    CATALOG_ENTRY_TYPE_FUNCTION,      // 函数条目
};

/**
 * @brief 目录条目基类
 */
class CatalogEntry
{
public:
    CatalogEntry(CatalogEntryType type, const std::string & name);

    virtual ~CatalogEntry() noexcept = default;

public:
    CatalogEntryType type() const noexcept;

    const std::string & name() const noexcept;

private:
    CatalogEntryType type_;         // 目录条目类型
    std::string name_;              // 目录条目名称
};

} // namespace dreamdb
