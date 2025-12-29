#include "dreamdb/index/hash_index.h"

#include <algorithm>

namespace dreamdb
{

HashIndex::HashIndex(const std::string & index_name, bool is_unique)
    : index_name_(index_name)
    , is_unique_(is_unique)
    , index_map_()
{
}

void HashIndex::set_index_name(const std::string & index_name) noexcept
{
    index_name_ = index_name;
}

void HashIndex::set_is_unique(bool is_unique) noexcept
{
    is_unique_ = is_unique;
}

const std::string & HashIndex::get_index_name() const
{
    return index_name_;
}

bool HashIndex::get_is_unique() const noexcept
{
    return is_unique_;
}

bool HashIndex::insert(std::unique_ptr<HashableIndexKeyBase> key, std::size_t entity_id)
{
    if (key == nullptr) {
        return false;
    }

    // 尝试查找原索引项
    auto it = index_map_.find(key);
    if (is_unique_ && it != index_map_.end()) {
        // 唯一索引，且已存在相同键，插入失败
        return false;
    }

    // 执行插入操作：将 entity_id 添加到对应键的 vector 中
    auto [insert_it, inserted] = index_map_.emplace(std::move(key), std::vector<std::size_t>());
    insert_it->second.push_back(entity_id);
    return true;
}

bool HashIndex::remove(std::unique_ptr<HashableIndexKeyBase> key, std::size_t entity_id)
{
    auto it = index_map_.find(key);
    if (it == index_map_.end()) {
        return false;
    }

    // 从 vector 中删除指定的 entity_id
    auto & entity_ids = it->second;
    auto erase_it = std::remove(entity_ids.begin(), entity_ids.end(), entity_id);
    entity_ids.erase(erase_it, entity_ids.end());

    // 如果 vector 为空，删除整个条目
    if (entity_ids.empty()) {
        index_map_.erase(it);
    }

    return true;
}

bool HashIndex::update(std::unique_ptr<HashableIndexKeyBase> old_key, std::unique_ptr<HashableIndexKeyBase> new_key, std::size_t entity_id)
{
    // 先删除旧键
    if (!remove(std::move(old_key), entity_id)) {
        return false;
    }

    // 再插入新键
    return insert(std::move(new_key), entity_id);
}

std::vector<std::size_t> HashIndex::search_equal(std::unique_ptr<HashableIndexKeyBase> key) const
{
    auto it = index_map_.find(key);
    if (it == index_map_.end()) {
        return {};
    }
    return it->second;
}

} // namespace dreamdb
