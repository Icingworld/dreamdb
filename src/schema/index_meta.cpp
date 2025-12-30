#include "dreamdb/schema/index_meta.h"

#include <algorithm>

namespace dreamdb
{

IndexMeta::IndexMeta()
    : index_name_(""),
      index_type_(IndexType::BTREE),
      is_unique_(false),
      field_indexes_()
{
}

void IndexMeta::set_index_name(const std::string & index_name)
{
    index_name_ = index_name;
}

void IndexMeta::set_index_type(IndexType index_type) noexcept
{
    index_type_ = index_type;
}

void IndexMeta::set_is_unique(bool is_unique) noexcept
{
    is_unique_ = is_unique;
}

void IndexMeta::add_field_index(std::size_t field_index)
{
    field_indexes_.push_back(field_index);
}

const std::string & IndexMeta::get_index_name() const noexcept
{
    return index_name_;
}

IndexType IndexMeta::get_index_type() const noexcept
{
    return index_type_;
}

bool IndexMeta::get_is_unique() const noexcept
{
    return is_unique_;
}

const std::vector<std::size_t> & IndexMeta::get_field_indexes() const noexcept
{
    return field_indexes_;
}

bool IndexMeta::has_field_index(std::size_t field_index) const noexcept
{
    return std::find(field_indexes_.begin(), field_indexes_.end(), field_index) != field_indexes_.end();
}

} // namespace dreamdb
