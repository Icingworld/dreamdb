#include "dreamdb/index/b_tree_index.h"

#include <algorithm>

namespace dreamdb
{

BTreeIndex::BTreeIndex(const std::string & index_name, bool is_unique)
    : index_name_(index_name)
    , is_unique_(is_unique)
    , index_map_()
{
}

void BTreeIndex::set_index_name(const std::string & index_name) noexcept
{
    index_name_ = index_name;
}

void BTreeIndex::set_is_unique(bool is_unique) noexcept
{
    is_unique_ = is_unique;
}

const std::string & BTreeIndex::get_index_name() const
{
    return index_name_;
}

bool BTreeIndex::get_is_unique() const noexcept
{
    return is_unique_;
}

bool BTreeIndex::insert(std::unique_ptr<ComparableIndexKeyBase> key, std::size_t entity_id)
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

bool BTreeIndex::remove(std::unique_ptr<ComparableIndexKeyBase> key, std::size_t entity_id)
{
    if (key == nullptr) {
        return false;
    }

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

bool BTreeIndex::update(std::unique_ptr<ComparableIndexKeyBase> old_key, std::unique_ptr<ComparableIndexKeyBase> new_key, std::size_t entity_id)
{
    // 先删除旧键
    if (!remove(std::move(old_key), entity_id)) {
        return false;
    }

    // 再插入新键
    return insert(std::move(new_key), entity_id);
}

std::vector<std::size_t> BTreeIndex::search_equal(std::unique_ptr<ComparableIndexKeyBase> key) const
{
    if (key == nullptr) {
        return {};
    }

    auto it = index_map_.find(key);
    if (it == index_map_.end()) {
        return {};
    }
    return it->second;
}

std::vector<std::size_t> BTreeIndex::search_greater_than(std::unique_ptr<ComparableIndexKeyBase> key) const
{
    if (key == nullptr) {
        return {};
    }

    std::vector<std::size_t> result;

    // 找到第一个大于 key 的位置
    // 直接传递 key，不需要 move，因为查找操作不需要转移所有权
    auto it = index_map_.upper_bound(key);

    // 从该位置开始，收集所有后续条目的 entity_id
    for (; it != index_map_.end(); ++it) {
        const auto & entity_ids = it->second;
        result.insert(result.end(), entity_ids.begin(), entity_ids.end());
    }

    return result;
}

std::vector<std::size_t> BTreeIndex::search_less_than(std::unique_ptr<ComparableIndexKeyBase> key) const
{
    if (key == nullptr) {
        return {};
    }

    std::vector<std::size_t> result;

    // 找到第一个大于等于 key 的位置（lower_bound）
    // 该位置之前的所有元素都小于 key
    auto it = index_map_.lower_bound(key);

    // 从开始到该位置，收集所有 entity_id
    for (auto begin_it = index_map_.begin(); begin_it != it; ++begin_it) {
        const auto & entity_ids = begin_it->second;
        result.insert(result.end(), entity_ids.begin(), entity_ids.end());
    }

    return result;
}

std::vector<std::size_t> BTreeIndex::search_greater_equal(std::unique_ptr<ComparableIndexKeyBase> key) const
{
    if (key == nullptr) {
        return {};
    }

    std::vector<std::size_t> result;

    // 找到第一个大于等于 key 的位置
    auto it = index_map_.lower_bound(key);

    // 从该位置开始，收集所有后续条目的 entity_id
    for (; it != index_map_.end(); ++it) {
        const auto & entity_ids = it->second;
        result.insert(result.end(), entity_ids.begin(), entity_ids.end());
    }

    return result;
}

std::vector<std::size_t> BTreeIndex::search_less_equal(std::unique_ptr<ComparableIndexKeyBase> key) const
{
    if (key == nullptr) {
        return {};
    }

    std::vector<std::size_t> result;

    // 找到第一个大于 key 的位置（upper_bound）
    // 该位置之前的所有元素都小于等于 key
    auto it = index_map_.upper_bound(key);

    // 从开始到该位置，收集所有 entity_id
    for (auto begin_it = index_map_.begin(); begin_it != it; ++begin_it) {
        const auto & entity_ids = begin_it->second;
        result.insert(result.end(), entity_ids.begin(), entity_ids.end());
    }

    return result;
}

std::vector<std::size_t> BTreeIndex::search_between(std::unique_ptr<ComparableIndexKeyBase> left_key, std::unique_ptr<ComparableIndexKeyBase> right_key) const
{
    if (left_key == nullptr || right_key == nullptr) {
        return {};
    }

    std::vector<std::size_t> result;

    // 找到左边界（大于等于 left_key）
    auto left_it = index_map_.lower_bound(left_key);

    // 找到右边界（大于 right_key）
    auto right_it = index_map_.upper_bound(right_key);

    // 在范围内收集所有 entity_id
    for (; left_it != right_it; ++left_it) {
        const auto & entity_ids = left_it->second;
        result.insert(result.end(), entity_ids.begin(), entity_ids.end());
    }

    return result;
}

} // namespace dreamdb
