#include "dreamdb/catalog/catalog_entry.h"

namespace dreamdb
{

CatalogEntry::CatalogEntry(CatalogEntryType type, const std::string & name)
    : type_(type)
    , name_(name)
{
}

CatalogEntryType CatalogEntry::type() const noexcept
{
    return type_;
}

const std::string & CatalogEntry::name() const noexcept
{
    return name_;
}

} // namespace dreamdb
