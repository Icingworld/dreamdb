#include "dreamdb/index/index_manager.h"

namespace dreamdb
{

bool IndexManager::register_index(const std::string & index_name, std::unique_ptr<IndexBase> index) noexcept
{
    return indexes_.insert({index_name, std::move(index)}).second;
}

bool IndexManager::unregister_index(const std::string & index_name) noexcept
{
    return indexes_.erase(index_name) > 0;
}

bool IndexManager::has_index(const std::string & index_name) const noexcept
{
    return indexes_.find(index_name) != indexes_.end();
}

bool IndexManager::insert_comparable_index(const std::string & index_name, std::unique_ptr<ComparableIndexKeyBase> key, std::size_t entity_id) noexcept
{
    auto it = indexes_.find(index_name);
    if (it == indexes_.end()) {
        return false;
    }

    auto * comparable_index = dynamic_cast<ComparableIndexBase *>(it->second.get());
    if (comparable_index == nullptr) {
        return false;
    }

    return comparable_index->insert(std::move(key), entity_id);
}

bool IndexManager::remove_comparable_index(const std::string & index_name, std::unique_ptr<ComparableIndexKeyBase> key, std::size_t entity_id) noexcept
{
    auto it = indexes_.find(index_name);
    if (it == indexes_.end()) {
        return false;
    }

    auto * comparable_index = dynamic_cast<ComparableIndexBase *>(it->second.get());
    if (comparable_index == nullptr) {
        return false;
    }

    return comparable_index->remove(std::move(key), entity_id);
}

bool IndexManager::update_comparable_index(const std::string & index_name, std::unique_ptr<ComparableIndexKeyBase> old_key, std::unique_ptr<ComparableIndexKeyBase> new_key, std::size_t entity_id) noexcept
{
    auto it = indexes_.find(index_name);
    if (it == indexes_.end()) {
        return false;
    }

    auto * comparable_index = dynamic_cast<ComparableIndexBase *>(it->second.get());
    if (comparable_index == nullptr) {
        return false;
    }

    return comparable_index->update(std::move(old_key), std::move(new_key), entity_id);
}

std::vector<std::size_t> IndexManager::search_comparable_index_equal(const std::string & index_name, std::unique_ptr<ComparableIndexKeyBase> key) const noexcept
{
    auto it = indexes_.find(index_name);
    if (it == indexes_.end()) {
        return {};
    }

    auto * comparable_index = dynamic_cast<ComparableIndexBase *>(it->second.get());
    if (comparable_index == nullptr) {
        return {};
    }

    return comparable_index->search_equal(std::move(key));
}

std::vector<std::size_t> IndexManager::search_comparable_index_greater_than(const std::string & index_name, std::unique_ptr<ComparableIndexKeyBase> key) const noexcept
{
    auto it = indexes_.find(index_name);
    if (it == indexes_.end()) {
        return {};
    }

    auto * comparable_index = dynamic_cast<ComparableIndexBase *>(it->second.get());
    if (comparable_index == nullptr) {
        return {};
    }

    return comparable_index->search_greater_than(std::move(key));
}

std::vector<std::size_t> IndexManager::search_comparable_index_less_than(const std::string & index_name, std::unique_ptr<ComparableIndexKeyBase> key) const noexcept
{
    auto it = indexes_.find(index_name);
    if (it == indexes_.end()) {
        return {};
    }

    auto * comparable_index = dynamic_cast<ComparableIndexBase *>(it->second.get());
    if (comparable_index == nullptr) {
        return {};
    }

    return comparable_index->search_less_than(std::move(key));
}

std::vector<std::size_t> IndexManager::search_comparable_index_greater_equal(const std::string & index_name, std::unique_ptr<ComparableIndexKeyBase> key) const noexcept
{
    auto it = indexes_.find(index_name);
    if (it == indexes_.end()) {
        return {};
    }

    auto * comparable_index = dynamic_cast<ComparableIndexBase *>(it->second.get());
    if (comparable_index == nullptr) {
        return {};
    }

    return comparable_index->search_greater_equal(std::move(key));
}

std::vector<std::size_t> IndexManager::search_comparable_index_less_equal(const std::string & index_name, std::unique_ptr<ComparableIndexKeyBase> key) const noexcept
{
    auto it = indexes_.find(index_name);
    if (it == indexes_.end()) {
        return {};
    }

    auto * comparable_index = dynamic_cast<ComparableIndexBase *>(it->second.get());
    if (comparable_index == nullptr) {
        return {};
    }

    return comparable_index->search_less_equal(std::move(key));
}

std::vector<std::size_t> IndexManager::search_comparable_index_between(const std::string & index_name, std::unique_ptr<ComparableIndexKeyBase> left_key, std::unique_ptr<ComparableIndexKeyBase> right_key) const noexcept
{
    auto it = indexes_.find(index_name);
    if (it == indexes_.end()) {
        return {};
    }

    auto * comparable_index = dynamic_cast<ComparableIndexBase *>(it->second.get());
    if (comparable_index == nullptr) {
        return {};
    }

    return comparable_index->search_between(std::move(left_key), std::move(right_key));
}

bool IndexManager::insert_hashable_index(const std::string & index_name, std::unique_ptr<HashableIndexKeyBase> key, std::size_t entity_id) noexcept
{
    auto it = indexes_.find(index_name);
    if (it == indexes_.end()) {
        return false;
    }

    auto * hashable_index = dynamic_cast<HashableIndexBase *>(it->second.get());
    if (hashable_index == nullptr) {
        return false;
    }

    return hashable_index->insert(std::move(key), entity_id);
}

bool IndexManager::remove_hashable_index(const std::string & index_name, std::unique_ptr<HashableIndexKeyBase> key, std::size_t entity_id) noexcept
{
    auto it = indexes_.find(index_name);
    if (it == indexes_.end()) {
        return false;
    }

    auto * hashable_index = dynamic_cast<HashableIndexBase *>(it->second.get());
    if (hashable_index == nullptr) {
        return false;
    }

    return hashable_index->remove(std::move(key), entity_id);
}

bool IndexManager::update_hashable_index(const std::string & index_name, std::unique_ptr<HashableIndexKeyBase> old_key, std::unique_ptr<HashableIndexKeyBase> new_key, std::size_t entity_id) noexcept
{
    auto it = indexes_.find(index_name);
    if (it == indexes_.end()) {
        return false;
    }

    auto * hashable_index = dynamic_cast<HashableIndexBase *>(it->second.get());
    if (hashable_index == nullptr) {
        return false;
    }

    return hashable_index->update(std::move(old_key), std::move(new_key), entity_id);
}

std::vector<std::size_t> IndexManager::search_hashable_index_equal(const std::string & index_name, std::unique_ptr<HashableIndexKeyBase> key) const noexcept
{
    auto it = indexes_.find(index_name);
    if (it == indexes_.end()) {
        return {};
    }

    auto * hashable_index = dynamic_cast<HashableIndexBase *>(it->second.get());
    if (hashable_index == nullptr) {
        return {};
    }

    return hashable_index->search_equal(std::move(key));
}

} // namespace dreamdb
