#include "dreamdb/catalog/catalog_index_entry.h"

#include <algorithm>

namespace dreamdb
{

CatalogIndexEntry::CatalogIndexEntry(
    const std::string& index_name,
    IndexType index_type,
    bool is_unique,
    const std::vector<std::size_t>& field_indexes
)
    : CatalogEntry(CatalogEntryType::CATALOG_ENTRY_TYPE_INDEX, index_name)
    , index_type_(index_type)
    , is_unique_(is_unique)
    , field_indexes_(field_indexes)
{
}

IndexType CatalogIndexEntry::index_type() const noexcept
{
    return index_type_;
}

bool CatalogIndexEntry::is_unique() const noexcept
{
    return is_unique_;
}

const std::vector<std::size_t>& CatalogIndexEntry::field_indexes() const noexcept
{
    return field_indexes_;
}

bool CatalogIndexEntry::has_field_index(std::size_t field_index) const noexcept
{
    return std::find(field_indexes_.begin(), field_indexes_.end(), field_index) != field_indexes_.end();
}

} // namespace dreamdb
