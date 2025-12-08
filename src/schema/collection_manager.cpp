#include "dreamdb/schema/collection_manager.h"

namespace dreamdb
{

CollectionManager::CollectionManager(StorageFactory storage_factory)
    : segment_manager_(std::make_unique<SegmentManager>(storage_factory))
{
}

Collection * CollectionManager::create_collection(const std::string & name, const std::vector<Field> & schema)
{
    // 检查集合是否已存在
    if (has_collection(name)) {
        return nullptr;
    }

    // 创建新集合，传入 SegmentManager 的引用
    auto collection = std::make_unique<Collection>(name, schema, *segment_manager_);
    auto * collection_ptr = collection.get();

    // 存储集合
    collections_[name] = std::move(collection);

    return collection_ptr;
}

bool CollectionManager::drop_collection(const std::string & name)
{
    auto it = collections_.find(name);
    if (it == collections_.end()) {
        return false;
    }

    // 删除集合的所有段（可选，取决于是否需要保留段数据）
    // segment_manager_->drop_collection_segments(name);

    // 删除集合
    collections_.erase(it);

    return true;
}

Collection * CollectionManager::get_collection(const std::string & name)
{
    auto it = collections_.find(name);
    if (it == collections_.end()) {
        return nullptr;
    }

    return it->second.get();
}

const Collection * CollectionManager::get_collection(const std::string & name) const
{
    auto it = collections_.find(name);
    if (it == collections_.end()) {
        return nullptr;
    }

    return it->second.get();
}

bool CollectionManager::has_collection(const std::string & name) const
{
    return collections_.find(name) != collections_.end();
}

std::vector<std::string> CollectionManager::list_collections() const
{
    std::vector<std::string> names;
    names.reserve(collections_.size());

    for (const auto & [name, collection] : collections_) {
        names.push_back(name);
    }

    return names;
}

std::size_t CollectionManager::collection_count() const noexcept
{
    return collections_.size();
}

SegmentManager & CollectionManager::get_segment_manager() noexcept
{
    return *segment_manager_;
}

const SegmentManager & CollectionManager::get_segment_manager() const noexcept
{
    return *segment_manager_;
}

void CollectionManager::set_storage_factory(StorageFactory factory)
{
    segment_manager_->set_storage_factory(factory);
}

} // namespace dreamdb

