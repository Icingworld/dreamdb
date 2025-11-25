#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include "dreamdb/schema/field.h"
#include "dreamdb/schema/entity.h"
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
     */
    Collection(
        const std::string & name, 
        const std::vector<Field> & schema
    );

    Collection(const Collection & other) = default;

    Collection(Collection && other) noexcept = default;

    Collection & operator=(Collection & other) = default;

    Collection & operator=(Collection && other) noexcept = default;

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
    std::string name;               // 集合名称
    std::vector<Field> schema;      // 字段定义列表
    std::int64_t next_id;           // 自增 ID 生成器
};

} // namespace dreamdb
