#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <memory>
#include <optional>

#include "dreamdb/schema/field.h"
#include "dreamdb/schema/entity.h"
#include "dreamdb/common/mutation_result.h"
#include "dreamdb/query/query.h"

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
    MutationResult remove(std::int64_t id);

    /**
     * @brief 更新实体
     * @param id 要更新的实体的 ID
     * @param fields 要更新的字段列表
     * @return 更新结果
     */
    MutationResult update(std::int64_t id, std::vector<std::pair<std::size_t, FieldValue>> fields);

    /**
     * @brief 查询实体
     * @param query 查询条件
     * @return 查询结果
     */
    std::vector<std::unique_ptr<Entity>> query(const Query & query) const;

private:
    std::string name_;                                  // 集合名称
    std::vector<Field> schema_;                         // 字段定义列表
    std::unordered_map<std::string, std::size_t> field_index_map_; // 字段索引映射
    std::int64_t next_id_;                              // 自增 ID 生成器
    std::unique_ptr<SegmentManager> segment_manager_;   // 段管理器
    std::shared_ptr<Segment> active_segment_;           // 当前活动段
};

} // namespace dreamdb
