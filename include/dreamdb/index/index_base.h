#pragma once

#include <cstddef>
#include <vector>
#include <memory>

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
};

/**
 * @brief 可比较的索引基类
 * @details 用于 B-tree 等需要排序的索引
 */
class ComparableIndexBase : public IndexBase
{
public:
    virtual ~ComparableIndexBase() = default;

public:
    /** 索引操作接口 */

    /**
     * @brief 插入一个索引项
     * @param key 索引键
     * @param entity_id 实体 ID
     * @return 是否插入成功
     */
    virtual bool insert(std::unique_ptr<ComparableIndexKeyBase> key, std::size_t entity_id) = 0;

    /**
     * @brief 删除指定索引项
     * @param key 索引键
     * @param entity_id 实体 ID
     * @return 是否删除成功
     */
    virtual bool remove(std::unique_ptr<ComparableIndexKeyBase> key, std::size_t entity_id) = 0;

    /**
     * @brief 更新指定索引项
     * @param old_key 旧索引键
     * @param new_key 新索引键
     * @param entity_id 实体 ID
     * @return 是否更新成功
     */
    virtual bool update(std::unique_ptr<ComparableIndexKeyBase> old_key, std::unique_ptr<ComparableIndexKeyBase> new_key, std::size_t entity_id) = 0;

    /** 索引查询接口 */

    /**
     * @brief 查询等于指定索引键的实体 ID 列表
     * @param key 索引键
     * @return 实体 ID 列表
     */
    virtual std::vector<std::size_t> search_equal(std::unique_ptr<ComparableIndexKeyBase> key) const = 0;

    /**
     * @brief 查询大于指定索引键的实体 ID 列表
     * @param key 索引键
     * @return 实体 ID 列表
     */
    virtual std::vector<std::size_t> search_greater_than(std::unique_ptr<ComparableIndexKeyBase> key) const = 0;

    /**
     * @brief 查询小于指定索引键的实体 ID 列表
     * @param key 索引键
     * @return 实体 ID 列表
     */
    virtual std::vector<std::size_t> search_less_than(std::unique_ptr<ComparableIndexKeyBase> key) const = 0;

    /**
     * @brief 查询大于等于指定索引键的实体 ID 列表
     * @param key 索引键
     * @return 实体 ID 列表
     */
    virtual std::vector<std::size_t> search_greater_equal(std::unique_ptr<ComparableIndexKeyBase> key) const = 0;

    /**
     * @brief 查询小于等于指定索引键的实体 ID 列表
     * @param key 索引键
     * @return 实体 ID 列表
     */
    virtual std::vector<std::size_t> search_less_equal(std::unique_ptr<ComparableIndexKeyBase> key) const = 0;

    /**
     * @brief 查询指定索引键范围内的实体 ID 列表
     * @param left_key 左边界索引键
     * @param right_key 右边界索引键
     * @return 实体 ID 列表
     */
    virtual std::vector<std::size_t> search_between(std::unique_ptr<ComparableIndexKeyBase> left_key, std::unique_ptr<ComparableIndexKeyBase> right_key) const = 0;
};

/**
 * @brief 可哈希的索引基类
 * @details 用于 Hash 索引
 */
class HashableIndexBase : public IndexBase
{
public:
    virtual ~HashableIndexBase() = default;

public:
    /** 索引操作接口 */

    /**
     * @brief 插入一个索引项
     * @param key 索引键
     * @param entity_id 实体 ID
     * @return 是否插入成功
     */
    virtual bool insert(std::unique_ptr<HashableIndexKeyBase> key, std::size_t entity_id) = 0;

    /**
     * @brief 删除指定索引项
     * @param key 索引键
     * @param entity_id 实体 ID
     * @return 是否删除成功
     */
    virtual bool remove(std::unique_ptr<HashableIndexKeyBase> key, std::size_t entity_id) = 0;

    /**
     * @brief 更新指定索引项
     * @param old_key 旧索引键
     * @param new_key 新索引键
     * @param entity_id 实体 ID
     * @return 是否更新成功
     */
    virtual bool update(std::unique_ptr<HashableIndexKeyBase> old_key, std::unique_ptr<HashableIndexKeyBase> new_key, std::size_t entity_id) = 0;

    /** 索引查询接口 */

    /**
     * @brief 查询等于指定索引键的实体 ID 列表
     * @param key 索引键
     * @return 实体 ID 列表
     */
    virtual std::vector<std::size_t> search_equal(std::unique_ptr<HashableIndexKeyBase> key) const = 0;
};

} // namespace dreamdb
