#include "dreamdb/schema/collection.h"

#include <unordered_set>
#include "dreamdb/storage/segment_manager.h"
#include "dreamdb/storage/segment.h"
#include "dreamdb/index/index_key_base.h"
#include "dreamdb/index/b_tree_index.h"
#include "dreamdb/index/hash_index.h"
#include "dreamdb/index/index_key.h"
#include "dreamdb/parser/ast/binary_expr.h"
#include "dreamdb/parser/ast/identifier_expr.h"
#include "dreamdb/parser/ast/literal_expr.h"

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
    // 简单优化：如果有 WHERE 条件，尝试使用索引
    if (query.has_where_clause()) {
        // 提取 WHERE 条件中的字段
        std::vector<std::size_t> query_fields = extract_fields_from_where(query.get_where_clause());
        
        if (!query_fields.empty()) {
            // 查找完全匹配的索引
            auto indexes = find_index_metadata_by_fields(query_fields);
            for (const auto* index_meta : indexes) {
                if (index_meta != nullptr && 
                    index_meta->get_field_indexes() == query_fields) {
                    // 使用索引查询
                    return query_using_index(query, index_meta);
                }
            }
        }
    }

    // 回退到全表扫描
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
    // 从 entity 中提取 index_meta.get_field_indexes() 对应的字段值
    std::vector<FieldValue> values;
    values.reserve(index_meta.get_field_indexes().size());
    for (std::size_t field_index : index_meta.get_field_indexes()) {
        values.push_back(entity.get_value(field_index));
    }
    
    // 创建 IndexKey 对象（同时实现 ComparableIndexKeyBase 和 HashableIndexKeyBase）
    return std::make_unique<IndexKey>(values);
}

std::unique_ptr<HashableIndexKeyBase> Collection::create_hashable_index_key(
    const Entity& entity,
    const IndexMeta& index_meta
) const
{
    // 从 entity 中提取 index_meta.get_field_indexes() 对应的字段值
    std::vector<FieldValue> values;
    values.reserve(index_meta.get_field_indexes().size());
    for (std::size_t field_index : index_meta.get_field_indexes()) {
        values.push_back(entity.get_value(field_index));
    }
    
    // 创建 IndexKey 对象（同时实现 ComparableIndexKeyBase 和 HashableIndexKeyBase）
    return std::make_unique<IndexKey>(values);
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

std::vector<std::size_t> Collection::extract_fields_from_where(const AstNode* where_clause) const
{
    std::vector<std::size_t> field_indices;
    
    if (where_clause == nullptr) {
        return field_indices;
    }
    
    // 仅支持简单的等值查询：field = value
    // 必须是 BinaryExpr，操作符是 DB_EQUAL，左操作数是 IdentifierExpr，右操作数是 LiteralExpr
    if (where_clause->get_type() != AstNodeType::BINARY_EXPR) {
        return field_indices;
    }
    
    const BinaryExpr* binary_expr = static_cast<const BinaryExpr*>(where_clause);
    
    // 只处理等值查询
    if (binary_expr->get_operator_type() != BinaryExpr::OperatorType::DB_EQUAL) {
        return field_indices;
    }
    
    // 检查左操作数是否是标识符（字段名）
    const AstNode* left = binary_expr->get_left();
    if (left == nullptr || left->get_type() != AstNodeType::IDENTIFIER_EXPR) {
        return field_indices;
    }
    
    const IdentifierExpr* identifier = static_cast<const IdentifierExpr*>(left);
    if (identifier->get_identifier_type() != IdentifierExpr::IdentifierType::COLUMN) {
        return field_indices;
    }
    
    // 获取字段名
    const std::string& field_name = identifier->get_original_identifier();
    std::optional<std::size_t> field_index = get_field_index(field_name);
    
    if (field_index.has_value()) {
        field_indices.push_back(field_index.value());
    }
    
    return field_indices;
}

std::unique_ptr<IndexKey> Collection::build_index_key_from_where(
    const AstNode* where_clause,
    const IndexMeta& index_meta
) const
{
    if (where_clause == nullptr) {
        return nullptr;
    }
    
    // 仅支持简单的等值查询：field = value
    if (where_clause->get_type() != AstNodeType::BINARY_EXPR) {
        return nullptr;
    }
    
    const BinaryExpr* binary_expr = static_cast<const BinaryExpr*>(where_clause);
    
    // 只处理等值查询
    if (binary_expr->get_operator_type() != BinaryExpr::OperatorType::DB_EQUAL) {
        return nullptr;
    }
    
    // 检查是否是单字段索引的等值查询
    const std::vector<std::size_t>& index_fields = index_meta.get_field_indexes();
    if (index_fields.size() != 1) {
        // 目前只支持单字段索引的等值查询
        return nullptr;
    }
    
    // 检查左操作数是否是标识符（字段名）
    const AstNode* left = binary_expr->get_left();
    if (left == nullptr || left->get_type() != AstNodeType::IDENTIFIER_EXPR) {
        return nullptr;
    }
    
    const IdentifierExpr* identifier = static_cast<const IdentifierExpr*>(left);
    if (identifier->get_identifier_type() != IdentifierExpr::IdentifierType::COLUMN) {
        return nullptr;
    }
    
    // 检查字段是否匹配索引
    const std::string& field_name = identifier->get_original_identifier();
    std::optional<std::size_t> field_index = get_field_index(field_name);
    
    if (!field_index.has_value() || field_index.value() != index_fields[0]) {
        return nullptr;
    }
    
    // 检查右操作数是否是字面量
    const AstNode* right = binary_expr->get_right();
    if (right == nullptr || right->get_type() != AstNodeType::LITERAL_EXPR) {
        return nullptr;
    }
    
    const LiteralExpr* literal = static_cast<const LiteralExpr*>(right);
    const auto& literal_value = literal->get_literal_value();
    LiteralExpr::LiteralType literal_type = literal->get_literal_type();
    
    // 根据字面量类型和目标字段类型进行转换
    FieldType target_field_type = schema_[field_index.value()].get_type();
    FieldValue field_value;
    
    switch (literal_type) {
        case LiteralExpr::LiteralType::INTEGER: {
            std::int64_t int_val = std::get<std::int64_t>(literal_value);
            switch (target_field_type) {
                case FieldType::TINYINT:
                    field_value = static_cast<std::int8_t>(int_val);
                    break;
                case FieldType::SMALLINT:
                    field_value = static_cast<std::int16_t>(int_val);
                    break;
                case FieldType::INTEGER:
                    field_value = static_cast<std::int32_t>(int_val);
                    break;
                case FieldType::BIGINT:
                case FieldType::TIMESTAMP:
                    field_value = int_val;
                    break;
                case FieldType::FLOAT:
                    field_value = static_cast<float>(int_val);
                    break;
                case FieldType::DOUBLE:
                    field_value = static_cast<double>(int_val);
                    break;
                default:
                    field_value = int_val;
                    break;
            }
            break;
        }
        case LiteralExpr::LiteralType::FLOAT: {
            double float_val = std::get<double>(literal_value);
            switch (target_field_type) {
                case FieldType::FLOAT:
                    field_value = static_cast<float>(float_val);
                    break;
                case FieldType::DOUBLE:
                    field_value = float_val;
                    break;
                case FieldType::TINYINT:
                    field_value = static_cast<std::int8_t>(float_val);
                    break;
                case FieldType::SMALLINT:
                    field_value = static_cast<std::int16_t>(float_val);
                    break;
                case FieldType::INTEGER:
                    field_value = static_cast<std::int32_t>(float_val);
                    break;
                case FieldType::BIGINT:
                    field_value = static_cast<std::int64_t>(float_val);
                    break;
                default:
                    field_value = float_val;
                    break;
            }
            break;
        }
        case LiteralExpr::LiteralType::STRING: {
            field_value = std::get<std::string>(literal_value);
            break;
        }
        case LiteralExpr::LiteralType::BOOLEAN: {
            field_value = std::get<bool>(literal_value);
            break;
        }
        case LiteralExpr::LiteralType::NULL_VALUE: {
            field_value = Null();
            break;
        }
        case LiteralExpr::LiteralType::VECTOR: {
            field_value = std::get<std::vector<float>>(literal_value);
            break;
        }
        default:
            return nullptr;
    }
    
    // 构建索引键
    std::vector<FieldValue> values;
    values.push_back(field_value);
    
    return std::make_unique<IndexKey>(values);
}

std::vector<std::unique_ptr<Entity>> Collection::query_using_index(
    const Query& query,
    const IndexMeta* index_meta
) const
{
    if (index_meta == nullptr) {
        return {};
    }
    
    // 从 WHERE 条件构建索引键
    std::unique_ptr<IndexKey> key = build_index_key_from_where(
        query.get_where_clause(),
        *index_meta
    );
    
    if (key == nullptr) {
        // 无法构建索引键，回退到全表扫描
        return active_segment_->query(query, this);
    }
    
    // 使用索引查找
    std::vector<std::size_t> entity_ids;
    if (index_meta->get_index_type() == IndexType::HASH) {
        // IndexKey 同时继承自 HashableIndexKeyBase，可以直接转换
        std::unique_ptr<HashableIndexKeyBase> hashable_key(key.release());
        entity_ids = index_manager_->search_hashable_index_equal(
            index_meta->get_index_name(),
            std::move(hashable_key)
        );
    } else if (index_meta->get_index_type() == IndexType::BTREE) {
        // IndexKey 同时继承自 ComparableIndexKeyBase，可以直接转换
        std::unique_ptr<ComparableIndexKeyBase> comparable_key(key.release());
        entity_ids = index_manager_->search_comparable_index_equal(
            index_meta->get_index_name(),
            std::move(comparable_key)
        );
    } else {
        // 不支持的索引类型，回退到全表扫描
        return active_segment_->query(query, this);
    }
    
    // 根据 entity_id 获取实体
    std::vector<std::unique_ptr<Entity>> results;
    results.reserve(entity_ids.size());
    
    for (std::size_t id : entity_ids) {
        auto entity = active_segment_->get_by_id(id);
        if (entity != nullptr) {
            results.push_back(std::move(entity));
        }
    }
    
    // TODO: 这里应该应用 ORDER BY 和 LIMIT，但目前先简单返回所有结果
    // 后续可以在这里添加排序和限制逻辑
    
    return results;
}

} // namespace dreamdb
