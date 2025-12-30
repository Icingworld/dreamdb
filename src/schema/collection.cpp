#include "dreamdb/schema/collection.h"

#include <algorithm>
#include <unordered_set>
#include "dreamdb/storage/segment_manager.h"
#include "dreamdb/storage/segment.h"
#include "dreamdb/index/index_key_base.h"
#include "dreamdb/index/b_tree_index.h"
#include "dreamdb/index/hash_index.h"
#include "dreamdb/index/index_key.h"

namespace dreamdb
{

Collection::Collection(
    const std::string & name,
    const std::vector<Field> & schema,
    std::unique_ptr<SegmentManager> segment_manager
)
    : name_(name)
    , schema_(schema)
    , field_index_map_()
    , next_id_(1)
    , segment_manager_(std::move(segment_manager))
    , active_segment_(segment_manager_->create_segment())
    , index_meta_map_()
    , index_manager_(std::make_shared<IndexManager>())
{
    // 初始化字段映射表
    for (std::size_t i = 0; i < schema.size(); ++i) {
        field_index_map_[schema[i].get_name()] = i;
    }
}

Collection::~Collection() = default;

void Collection::set_name(const std::string & name)
{
    name_ = name;
}

const std::string & Collection::get_name() const
{
    return name_;
}

const std::vector<Field> & Collection::get_schema() const
{
    return schema_;
}

std::optional<std::size_t> Collection::get_field_index(const std::string & name) const
{
    auto it = field_index_map_.find(name);
    if (it != field_index_map_.end()) {
        return it->second;
    }

    return std::nullopt;
}

Entity Collection::create_entity()
{
    // 预分配字段数量
    Entity entity(next_id_++, schema_.size());

    return entity;
}

MutationResult Collection::insert(const Entity & entity)
{
    // TODO: 检查是否需要 Seal 段
    return active_segment_->insert(entity);
}

MutationResult Collection::remove(std::size_t id)
{
    return active_segment_->remove_by_id(id);
}

MutationResult Collection::update(std::size_t id, std::vector<std::pair<std::size_t, FieldValue>> fields)
{
    return active_segment_->update_by_id(id, fields);
}

std::vector<std::unique_ptr<Entity>> Collection::query(const Query & query) const
{
    return active_segment_->query(query, this);
}

bool Collection::create_index(const IndexMeta & index_meta)
{
    const std::string & index_name = index_meta.get_index_name();

    // 检查索引是否已存在
    if (index_meta_map_.find(index_name) != index_meta_map_.end()) {
        return false;
    }

    // 添加索引元数据（使用移动构造）
    IndexMeta meta_copy;
    meta_copy.set_index_name(index_meta.get_index_name());
    meta_copy.set_index_type(index_meta.get_index_type());
    meta_copy.set_is_unique(index_meta.get_is_unique());
    for (std::size_t field_index : index_meta.get_field_indexes()) {
        meta_copy.add_field_index(field_index);
    }
    index_meta_map_.emplace(index_name, std::move(meta_copy));

    // 重建字段到索引的映射
    rebuild_field_to_index_map();

    // 创建索引
    std::unique_ptr<IndexBase> index;
    if (index_meta.get_index_type() == IndexType::BTREE) {
        index = std::make_unique<BTreeIndex>(index_name, index_meta.get_is_unique());
    } else if (index_meta.get_index_type() == IndexType::HASH) {
        index = std::make_unique<HashIndex>(index_name, index_meta.get_is_unique());
    } else {
        return false;
    }

    // 注册索引
    if (!index_manager_->register_index(index_name, std::move(index))) {
        // 回滚：删除已添加的元数据
        index_meta_map_.erase(index_name);
        rebuild_field_to_index_map();
        return false;
    }

    // 扫描集合，为现有数据构建索引
    Query query;
    std::vector<std::unique_ptr<Entity>> entities = active_segment_->query(query, this);
    for (const std::unique_ptr<Entity> & entity : entities) {
        // 选取相应的字段值构建索引键
        std::vector<FieldValue> values;
        values.reserve(index_meta.get_field_indexes().size());
        for (std::size_t field_index : index_meta.get_field_indexes()) {
            values.push_back(entity->get_value(field_index));
        }
        
        std::size_t entity_id = entity->get_id();
        
        // 根据索引类型插入索引键
        if (index_meta.get_index_type() == IndexType::BTREE) {
            // B-Tree 索引：使用 ComparableIndexKeyBase
            auto key = std::make_unique<IndexKey>(values);
            if (!index_manager_->insert_comparable_index(index_name, std::move(key), entity_id)) {
                // 插入失败，回滚
                index_manager_->unregister_index(index_name);
                index_meta_map_.erase(index_name);
                rebuild_field_to_index_map();
                return false;
            }
        } else if (index_meta.get_index_type() == IndexType::HASH) {
            // Hash 索引：使用 HashableIndexKeyBase
            auto key = std::make_unique<IndexKey>(values);
            if (!index_manager_->insert_hashable_index(index_name, std::move(key), entity_id)) {
                // 插入失败，回滚
                index_manager_->unregister_index(index_name);
                index_meta_map_.erase(index_name);
                rebuild_field_to_index_map();
                return false;
            }
        } else {
            // 不支持的索引类型，回滚
            index_manager_->unregister_index(index_name);
            index_meta_map_.erase(index_name);
            rebuild_field_to_index_map();
            return false;
        }
    }

    return true;
}

bool Collection::remove_index(const std::string & index_name)
{
    auto it = index_meta_map_.find(index_name);
    if (it == index_meta_map_.end()) {
        return false;
    }

    // 从 IndexManager 中删除索引
    index_manager_->unregister_index(index_name);

    // 删除索引元数据
    index_meta_map_.erase(it);

    // 重建字段到索引的映射
    rebuild_field_to_index_map();

    return true;
}

std::optional<IndexMeta> Collection::get_index_meta(const std::string & index_name) const
{
    auto it = index_meta_map_.find(index_name);
    if (it != index_meta_map_.end()) {
        // 由于 IndexMeta 的拷贝构造函数被删除，需要手动构造
        IndexMeta result;
        result.set_index_name(it->second.get_index_name());
        result.set_index_type(it->second.get_index_type());
        result.set_is_unique(it->second.get_is_unique());
        for (std::size_t field_index : it->second.get_field_indexes()) {
            result.add_field_index(field_index);
        }
        return result;
    }
    return std::nullopt;
}

std::vector<const IndexMeta*> Collection::find_index_metadata_by_fields(
    const std::vector<std::size_t>& field_indices
) const
{
    std::unordered_set<const IndexMeta*> result_set;  // 用于去重
    
    // 遍历每个字段，收集相关索引
    for (std::size_t field_index : field_indices) {
        auto it = field_to_index_map_.find(field_index);
        if (it != field_to_index_map_.end()) {
            for (const IndexMeta* meta : it->second) {
                result_set.insert(meta);
            }
        }
    }
    
    // 转换为向量返回
    std::vector<const IndexMeta*> result;
    result.reserve(result_set.size());
    result.assign(result_set.begin(), result_set.end());
    
    return result;
}

std::vector<const IndexMeta*> Collection::find_index_metadata_by_field(
    std::size_t field_index
) const
{
    auto it = field_to_index_map_.find(field_index);
    if (it != field_to_index_map_.end()) {
        return it->second;
    }
    return {};
}

std::vector<const IndexMeta*> Collection::get_all_index_metadata() const
{
    std::vector<const IndexMeta*> result;
    result.reserve(index_meta_map_.size());
    
    for (const auto& [index_name, index_meta] : index_meta_map_) {
        result.push_back(&index_meta);
    }
    
    return result;
}

const IndexMeta* Collection::get_index_metadata(const std::string& index_name) const
{
    auto it = index_meta_map_.find(index_name);
    if (it != index_meta_map_.end()) {
        return &it->second;
    }
    return nullptr;
}

void Collection::rebuild_field_to_index_map()
{
    field_to_index_map_.clear();
    
    for (const auto& [index_name, index_meta] : index_meta_map_) {
        for (std::size_t field_index : index_meta.get_field_indexes()) {
            field_to_index_map_[field_index].push_back(&index_meta);
        }
    }
}

// 注意：以下方法需要 IndexKey 的具体实现，暂时提供框架
// 实际使用时需要创建 IndexKey 类同时实现 ComparableIndexKeyBase 和 HashableIndexKeyBase

std::unique_ptr<ComparableIndexKeyBase> Collection::create_comparable_index_key(
    const Entity& entity,
    const IndexMeta& index_meta
) const
{
    // TODO: 需要实现具体的 IndexKey 类
    // 这里暂时返回 nullptr，实际实现时需要：
    // 1. 从 entity 中提取 index_meta.get_field_indexes() 对应的字段值
    // 2. 创建 IndexKey 对象（同时实现 ComparableIndexKeyBase 和 HashableIndexKeyBase）
    // 3. 返回该对象
    (void)entity;
    (void)index_meta;
    return nullptr;
}

std::unique_ptr<HashableIndexKeyBase> Collection::create_hashable_index_key(
    const Entity& entity,
    const IndexMeta& index_meta
) const
{
    // TODO: 需要实现具体的 IndexKey 类
    // 同上
    (void)entity;
    (void)index_meta;
    return nullptr;
}

bool Collection::update_indexes_on_insert(
    const Entity& entity,
    const std::vector<const IndexMeta*>& index_metadata_list
)
{
    for (const IndexMeta* meta : index_metadata_list) {
        if (meta == nullptr) continue;
        
        // 根据索引类型创建对应的键
        if (meta->get_index_type() == IndexType::BTREE) {
            auto key = create_comparable_index_key(entity, *meta);
            if (!key) continue;  // 暂时跳过，等待 IndexKey 实现
            if (!index_manager_->insert_comparable_index(
                meta->get_index_name(), std::move(key), entity.get_id())) {
                return false;
            }
        } else if (meta->get_index_type() == IndexType::HASH) {
            auto key = create_hashable_index_key(entity, *meta);
            if (!key) continue;  // 暂时跳过，等待 IndexKey 实现
            if (!index_manager_->insert_hashable_index(
                meta->get_index_name(), std::move(key), entity.get_id())) {
                return false;
            }
        }
    }
    return true;
}

bool Collection::update_indexes_on_remove(
    const Entity& entity,
    const std::vector<const IndexMeta*>& index_metadata_list
)
{
    for (const IndexMeta* meta : index_metadata_list) {
        if (meta == nullptr) continue;
        
        if (meta->get_index_type() == IndexType::BTREE) {
            auto key = create_comparable_index_key(entity, *meta);
            if (!key) continue;  // 暂时跳过，等待 IndexKey 实现
            if (!index_manager_->remove_comparable_index(
                meta->get_index_name(), std::move(key), entity.get_id())) {
                return false;
            }
        } else if (meta->get_index_type() == IndexType::HASH) {
            auto key = create_hashable_index_key(entity, *meta);
            if (!key) continue;  // 暂时跳过，等待 IndexKey 实现
            if (!index_manager_->remove_hashable_index(
                meta->get_index_name(), std::move(key), entity.get_id())) {
                return false;
            }
        }
    }
    return true;
}

bool Collection::update_indexes_on_update(
    const Entity& old_entity,
    const Entity& new_entity,
    const std::vector<const IndexMeta*>& index_metadata_list
)
{
    for (const IndexMeta* meta : index_metadata_list) {
        if (meta == nullptr) continue;
        
        if (meta->get_index_type() == IndexType::BTREE) {
            auto old_key = create_comparable_index_key(old_entity, *meta);
            auto new_key = create_comparable_index_key(new_entity, *meta);
            if (!old_key || !new_key) continue;  // 暂时跳过，等待 IndexKey 实现
            if (!index_manager_->update_comparable_index(
                meta->get_index_name(), 
                std::move(old_key), 
                std::move(new_key), 
                new_entity.get_id())) {
                return false;
            }
        } else if (meta->get_index_type() == IndexType::HASH) {
            auto old_key = create_hashable_index_key(old_entity, *meta);
            auto new_key = create_hashable_index_key(new_entity, *meta);
            if (!old_key || !new_key) continue;  // 暂时跳过，等待 IndexKey 实现
            if (!index_manager_->update_hashable_index(
                meta->get_index_name(), 
                std::move(old_key), 
                std::move(new_key), 
                new_entity.get_id())) {
                return false;
            }
        }
    }
    return true;
}

std::string Collection::check_unique_constraints(
    const Entity& entity,
    const std::vector<const IndexMeta*>& index_metadata_list
) const
{
    (void)entity;
    for (const IndexMeta* meta : index_metadata_list) {
        if (meta == nullptr || !meta->get_is_unique()) {
            continue;
        }
        
        // TODO: 需要实现索引的 contains 方法或类似方法
        // 这里暂时返回空字符串，实际实现时需要：
        // 1. 创建查询键（entity_id 设为 0）
        // 2. 调用 index_manager_->search_* 方法检查是否存在
        // 3. 如果存在，返回错误信息
    }
    return "";
}

} // namespace dreamdb
