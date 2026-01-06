#include "dreamdb/catalog/catalog_vindex_entry.h"

namespace dreamdb
{

CatalogVIndexEntry::CatalogVIndexEntry(
    const std::string& vindex_name,
    std::size_t column_index,
    VIndexType vindex_type,
    const std::optional<MetricType>& metric_type
)
    : CatalogEntry(CatalogEntryType::CATALOG_ENTRY_TYPE_VINDEX, vindex_name)
    , column_index_(column_index)
    , vindex_type_(vindex_type)
    , metric_type_(metric_type)
{
}

std::size_t CatalogVIndexEntry::column_index() const noexcept
{
    return column_index_;
}

VIndexType CatalogVIndexEntry::vindex_type() const noexcept
{
    return vindex_type_;
}

const std::optional<MetricType>& CatalogVIndexEntry::metric_type() const noexcept
{
    return metric_type_;
}

void CatalogVIndexEntry::set_metric_type(const std::optional<MetricType>& metric_type) noexcept
{
    metric_type_ = metric_type;
}

void CatalogVIndexEntry::set_option(const std::string& key, const std::string& value)
{
    options_[key] = value;
}

std::optional<std::string> CatalogVIndexEntry::get_option(const std::string& key) const
{
    auto it = options_.find(key);
    if (it == options_.end()) {
        return std::nullopt;
    }
    return it->second;
}

const std::unordered_map<std::string, std::string>& CatalogVIndexEntry::get_all_options() const noexcept
{
    return options_;
}

} // namespace dreamdb
