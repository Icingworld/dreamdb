#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <memory>

#include "dreamdb/schema/field.h"
#include "dreamdb/schema/entity.h"

namespace dreamdb
{

class SegmentManager;

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

    ~Collection() = default;

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

public:
    /** 实体操作接口 */

    /**
     * @brief 创建实体
     * @return 创建的实体
     */
    Entity create_entity();

private:
    std::string name_;                                  // 集合名称
    std::vector<Field> schema_;                         // 字段定义列表
    std::int64_t next_id_;                              // 自增 ID 生成器
    std::unique_ptr<SegmentManager> segment_manager_;   // 段管理器
};

} // namespace dreamdb
