#pragma once

#include <string>
#include <variant>
#include <vector>
#include <cstdint>

#include "dreamdb/schema/field.h"

namespace dreamdb
{

/**
 * @brief 实体类
 */
class Entity
{
public:
    Entity() = default;

    /**
     * @brief 创建实体
     * @param field_count 预分配字段数量
     */
    explicit Entity(std::size_t field_count);

    Entity(const Entity & other) = default;

    Entity(Entity && other) noexcept = default;

    Entity & operator=(const Entity & other) = default;

    Entity & operator=(Entity && other) noexcept = default;

    ~Entity() = default;

public:
    /** 属性值访问接口 */

    /**
     * @brief 设置字段值
     * @param index 字段索引
     * @param value 字段值
     * @throw std::out_of_range 如果索引越界
     */
    void set_value(std::size_t index, const FieldValue & value);

    /**
     * @brief 获取字段值
     * @param index 字段索引
     * @return 字段值
     * @throw std::out_of_range 如果索引越界
     */
    const FieldValue & get_value(std::size_t index) const;

    /**
     * @brief 获取字段值
     * @param index 字段索引
     * @return 字段值
     * @throw std::out_of_range 如果索引越界
     */
    FieldValue & get_value(std::size_t index);

    /**
     * @brief 获取字段值
     * @tparam T 期望的类型
     * @param index 字段索引
     * @return 字段值
     * @throw std::out_of_range 如果索引越界
     */
    template<typename T>
    T get_value_as(std::size_t index) const;

    /**
     * @brief 检查字段值是否为空
     * @param index 字段索引
     * @return 如果字段值为空返回 true
     * @throw std::out_of_range 如果索引越界
     */
    bool is_null(std::size_t index) const;

    /**
     * @brief 获取字段数量
     * @return 字段数量
     */
    std::size_t field_count() const;

    /**
     * @brief 清空所有字段值
     */
    void clear();

    /**
     * @brief 检查实体是否为空
     * @return 如果所有字段都是 null 返回 true
     */
    bool is_empty() const;

private:
    std::vector<FieldValue> values;  // 字段值列表，按索引顺序存储
};

} // namespace dreamdb

#include "dreamdb/schema/entity.inl"
