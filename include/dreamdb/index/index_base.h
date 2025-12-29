#pragma once

#include <cstddef>
#include <vector>

#include "dreamdb/index/index_key_base.h"

namespace dreamdb
{

/**
 * @brief 索引基类
 */
class IndexBase
{
public:
    virtual ~IndexBase() = default;

public:
    /** 索引操作接口 */

    /**
     * @brief 插入一个索引项
     * @param key 索引键
     * @param entity_id 实体 ID
     * @return 是否插入成功
     */
    virtual bool insert(const IndexKeyBase & key, std::size_t entity_id) = 0;

    /**
     * @brief 删除指定索引项
     * @param key 索引键
     * @param entity_id 实体 ID
     * @return 是否删除成功
     */
    virtual bool remove(const IndexKeyBase & key, std::size_t entity_id) = 0;

    /**
     * @brief 更新指定索引项
     * @param old_key 旧索引键
     * @param new_key 新索引键
     * @param entity_id 实体 ID
     * @return 是否更新成功
     */
    virtual bool update(const IndexKeyBase & old_key, const IndexKeyBase & new_key, std::size_t entity_id) = 0;

    /** 索引查询接口 */

    /**
     * @brief 查询等于指定索引键的实体 ID 列表
     * @param key 索引键
     * @return 实体 ID 列表
     */
    virtual std::vector<std::size_t> search_equal(const IndexKeyBase & key) const = 0;

    /**
     * @brief 查询大于指定索引键的实体 ID 列表
     * @param key 索引键
     * @return 实体 ID 列表
     */
    virtual std::vector<std::size_t> search_greater_than(const IndexKeyBase & key) const = 0;

    /**
     * @brief 查询小于指定索引键的实体 ID 列表
     * @param key 索引键
     * @return 实体 ID 列表
     */
    virtual std::vector<std::size_t> search_less_than(const IndexKeyBase & key) const = 0;

    /**
     * @brief 查询大于等于指定索引键的实体 ID 列表
     * @param key 索引键
     * @return 实体 ID 列表
     */
    virtual std::vector<std::size_t> search_greater_equal(const IndexKeyBase & key) const = 0;

    /**
     * @brief 查询小于等于指定索引键的实体 ID 列表
     * @param key 索引键
     * @return 实体 ID 列表
     */
    virtual std::vector<std::size_t> search_less_equal(const IndexKeyBase & key) const = 0;

    /**
     * @brief 查询指定索引键范围内的实体 ID 列表
     * @param left_key 左边界索引键
     * @param right_key 右边界索引键
     * @return 实体 ID 列表
     */
    virtual std::vector<std::size_t> search_between(const IndexKeyBase & left_key, const IndexKeyBase & right_key) const = 0;
};

} // namespace dreamdb
