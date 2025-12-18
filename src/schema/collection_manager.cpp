#include "dreamdb/schema/collection_manager.h"

#include "dreamdb/schema/collection.h"
#include "dreamdb/storage/segment_manager.h"

namespace dreamdb
{

CollectionManager::~CollectionManager() = default;

Collection * CollectionManager::create_collection(const std::string & name, const std::vector<Field> & schema)
{
    // 已存在则返回 nullptr
    if (collections_.find(name) != collections_.end()) {
        return nullptr;
    }

    // 为新集合创建独立的 SegmentManager
    auto segment_manager = std::make_unique<SegmentManager>();
    auto collection = std::make_unique<Collection>(name, schema, std::move(segment_manager));
    auto * collection_ptr = collection.get();

    collections_.emplace(name, std::move(collection));
    return collection_ptr;
}

bool CollectionManager::drop_collection(const std::string & name)
{
    auto it = collections_.find(name);
    if (it == collections_.end()) {
        return false;
    }

    // TODO: 存储层删除集合
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

bool CollectionManager::has_collection(const std::string & name) const
{
    return collections_.find(name) != collections_.end();
}

} // namespace dreamdb

