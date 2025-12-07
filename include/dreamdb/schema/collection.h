#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <optional>

#include "dreamdb/schema/field.h"
#include "dreamdb/schema/entity.h"
#include "dreamdb/storage/segment_manager.h"
#include "dreamdb/common/type.h"

namespace dreamdb
{

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
        SegmentManager & segment_manager
    );

    Collection(const Collection & other) = delete;

    Collection(Collection && other) noexcept = delete;

    Collection & operator=(const Collection & other) = delete;

    Collection & operator=(Collection && other) noexcept = delete;

    ~Collection() = default;

public:
    /** 属性值访问接口 */

    /**
     * @brief 设置集合名称
     * @param name 集合名称
     */
    void set_name(const std::string & name);

    /**
     * @brief 设置字段定义列表
     * @param schema 字段定义列表
     * @todo 暂不实现
     */
    void set_schema(const std::vector<Field> & schema);

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

public:
    /** 实体操作接口 */

    /**
     * @brief 创建实体
     * @return 创建的实体
     */
    Entity create_entity();

public:
    /** CRUD 接口 */

    /**
     * @brief 插入实体
     * @param entity 实体
     * @return 操作结果
     */
    MutationResult insert(const Entity & entity);

    /**
     * @brief 删除符合条件的实体
     * @param key 字段名
     * @param value 字段值
     * @return 操作结果
     */
    MutationResult remove(const std::string & key, const std::string & value);

    /**
     * @brief 更新或插入实体
     * @param key 字段名
     * @param value 字段值
     * @param entity 实体
     * @return 操作结果
     */
    MutationResult upsert(const std::string & key, const std::string & value, const Entity & entity);

private:
    std::string name;                       // 集合名称
    std::vector<Field> schema;              // 字段定义列表
    SegmentManager & segment_manager;       // 段管理器
    std::int64_t next_id;                   // 自增 ID 生成器

private:
    std::optional<std::size_t> find_field_index(const std::string & key) const;
    FieldValue parse_literal(const Field & field, const std::string & literal) const;
};

} // namespace dreamdb
