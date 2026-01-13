#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cstdint>
#include <memory>
#include <optional>

#include "dreamdb/schema/field.h"
#include "dreamdb/schema/entity.h"
#include "dreamdb/common/mutation_result.h"
#include "dreamdb/schema/index_meta.h"
#include "dreamdb/index/index_manager.h"
#include "dreamdb/index/index_key_base.h"
#include "dreamdb/parser/ast/ast_node.h"
#include "dreamdb/index/index_key.h"

namespace dreamdb
{

class SegmentManager;
class Segment;

/**
 * @brief 集合类
 */
class Collection
{
public:
    /**
     * @brief 构造函数
     * @param name 集合名称
     * @param schema 字段定义列表
     * @param segment_manager 段管理器
     */
    Collection(
        const std::string & name,
        const std::vector<Field> & schema,
        std::unique_ptr<SegmentManager> segment_manager
    );

    Collection(const Collection & other) = delete;

    Collection(Collection && other) noexcept = default;

    Collection & operator=(const Collection & other) = delete;

    Collection & operator=(Collection && other) noexcept = default;

    ~Collection();

public:
    /** 属性值访问接口 */

    /**
     * @brief 设置集合名称
     * @param name 集合名称
     */
    void set_name(const std::string & name);

    /**
     * @brief 获取集合名称
     * @return 集合名称
     */
    const std::string & get_name() const;

    /**
     * @brief 获取字段定义列表
     * @return 字段定义列表
     */
    const std::vector<Field> & get_schema() const;

    /**
     * @brief 获取字段索引
     * @param name 字段名称
     * @return 字段索引
     */
    std::optional<std::size_t> get_field_index(const std::string & name) const;

public:
    /** 实体操作接口 */

    /**
     * @brief 创建实体
     * @return 创建的实体
     */
    Entity create_entity();

public:
    /** CRUD 操作接口 */

    /**
     * @brief 插入实体
     * @param entity 要插入的实体
     * @return 插入结果
     */
    MutationResult insert(const Entity & entity);

    /**
     * @brief 删除实体
     * @param id 要删除的实体的 ID
     * @return 删除结果
     */
    MutationResult remove(std::size_t id);

    /**
     * @brief 更新实体
     * @param id 要更新的实体的 ID
     * @param fields 要更新的字段列表
     * @return 更新结果
     */
    MutationResult update(std::size_t id, std::vector<std::pair<std::size_t, FieldValue>> fields);

    /**
     * @brief 获取所有实体（用于全表扫描）
     * @return 所有实体的列表
     * @note 过滤、排序和限制由物理计划层处理
     */
    std::vector<std::unique_ptr<Entity>> get_all_entities() const;

    /** 索引接口 */

    /**
     * @brief 创建索引
     * @param index_meta 索引元数据
     * @return 是否创建成功
     */
    bool create_index(const IndexMeta & index_meta);

    /**
     * @brief 删除索引
     * @param index_name 索引名称
     * @return 是否删除成功
     */
    bool remove_index(const std::string & index_name);

    /**
     * @brief 查询索引
     * @param index_name 索引名称
     * @return 索引元数据
     */
    std::optional<IndexMeta> get_index_meta(const std::string & index_name) const;

    /** 索引元数据查询接口 */

    /**
     * @brief 根据字段索引列表查找相关的索引元数据
     * @param field_indices 字段索引列表（例如：[1, 3]）
     * @return 相关的索引元数据列表（去重后）
     * @details 返回所有包含这些字段中任意一个的索引
     */
    std::vector<const IndexMeta*> find_index_metadata_by_fields(
        const std::vector<std::size_t>& field_indices
    ) const;

    /**
     * @brief 根据单个字段索引查找相关的索引元数据
     * @param field_index 字段索引
     * @return 相关的索引元数据列表
     */
    std::vector<const IndexMeta*> find_index_metadata_by_field(
        std::size_t field_index
    ) const;

    /**
     * @brief 获取所有索引元数据
     * @return 索引元数据列表
     */
    std::vector<const IndexMeta*> get_all_index_metadata() const;

    /**
     * @brief 根据索引名称获取索引元数据
     * @param index_name 索引名称
     * @return 索引元数据指针，如果不存在返回 nullptr
     */
    const IndexMeta* get_index_metadata(const std::string& index_name) const;

    /** 索引键创建接口 */

    /**
     * @brief 从实体创建 ComparableIndexKey
     * @param entity 实体
     * @param index_meta 索引元数据
     * @return 索引键
     */
    std::unique_ptr<ComparableIndexKeyBase> create_comparable_index_key(
        const Entity& entity,
        const IndexMeta& index_meta
    ) const;

    /**
     * @brief 从实体创建 HashableIndexKey
     * @param entity 实体
     * @param index_meta 索引元数据
     * @return 索引键
     */
    std::unique_ptr<HashableIndexKeyBase> create_hashable_index_key(
        const Entity& entity,
        const IndexMeta& index_meta
    ) const;

    /** 索引批量操作接口 */

    /**
     * @brief 批量更新索引（插入操作）
     * @param entity 实体
     * @param index_metadata_list 需要更新的索引元数据列表
     * @return 是否全部成功
     */
    bool update_indexes_on_insert(
        const Entity& entity,
        const std::vector<const IndexMeta*>& index_metadata_list
    );

    /**
     * @brief 批量更新索引（删除操作）
     * @param entity 实体
     * @param index_metadata_list 需要更新的索引元数据列表
     * @return 是否全部成功
     */
    bool update_indexes_on_remove(
        const Entity& entity,
        const std::vector<const IndexMeta*>& index_metadata_list
    );

    /**
     * @brief 批量更新索引（更新操作）
     * @param old_entity 旧实体
     * @param new_entity 新实体
     * @param index_metadata_list 需要更新的索引元数据列表
     * @return 是否全部成功
     */
    bool update_indexes_on_update(
        const Entity& old_entity,
        const Entity& new_entity,
        const std::vector<const IndexMeta*>& index_metadata_list
    );

    /** 唯一性检查接口 */

    /**
     * @brief 检查唯一性约束
     * @param entity 实体
     * @param index_metadata_list 唯一索引元数据列表
     * @return 如果违反唯一性约束，返回错误信息；否则返回空字符串
     */
    std::string check_unique_constraints(
        const Entity& entity,
        const std::vector<const IndexMeta*>& index_metadata_list
    ) const;

private:
    /**
     * @brief 构建字段到索引的映射（内部辅助方法）
     * @details 在添加/删除索引时调用，用于加速查找
     */
    void rebuild_field_to_index_map();

    std::string name_;                                  // 集合名称
    std::vector<Field> schema_;                         // 字段定义列表
    std::unordered_map<std::string, std::size_t> field_index_map_; // 字段索引映射
    std::size_t next_id_;                               // 自增 ID 生成器
    std::unique_ptr<SegmentManager> segment_manager_;   // 段管理器
    std::shared_ptr<Segment> active_segment_;           // 当前活动段
    std::unordered_map<std::string, IndexMeta> index_meta_map_; // 索引元数据映射
    std::shared_ptr<IndexManager> index_manager_;               // 索引管理器
    
    // 字段到索引的映射（加速查找）
    // field_index -> [index_meta指针列表]
    std::unordered_map<std::size_t, std::vector<const IndexMeta*>> field_to_index_map_;
};

} // namespace dreamdb
