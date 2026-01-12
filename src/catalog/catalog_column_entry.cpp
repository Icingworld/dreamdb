#include "dreamdb/catalog/catalog_column_entry.h"

namespace dreamdb
{

CatalogColumnEntry::CatalogColumnEntry(const std::string & column_name, const LogicalType & logical_type, std::size_t column_index)
    : CatalogEntry(CatalogEntryType::CATALOG_ENTRY_TYPE_COLUMN, column_name)
    , logical_type_(logical_type)
    , column_index_(column_index)
{
}

const LogicalType & CatalogColumnEntry::logical_type() const noexcept
{
    return logical_type_;
}

std::size_t CatalogColumnEntry::column_index() const noexcept
{
    return column_index_;
}

} // namespace dreamdb