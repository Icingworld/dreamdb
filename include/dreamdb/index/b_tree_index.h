#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <map>

#include "dreamdb/index/index_base.h"
#include "dreamdb/index/index_key_base.h"

namespace dreamdb
{

/**
 * @brief ComparableIndexKeyBase 的比较函数
 */
struct ComparableIndexKeyCompare
{
    bool operator()(const std::unique_ptr<ComparableIndexKeyBase> & lhs, const std::unique_ptr<ComparableIndexKeyBase> & rhs) const
    {
        if (lhs == rhs) {
            return false;
        }
        if (lhs == nullptr) {
            // nullptr 小于任何非空指针
            return true;
        }
        if (rhs == nullptr) {
            // 任何非空指针大于 nullptr
            return false;
        }
        return lhs->compare(*rhs) < 0;
    }
};

/**
 * @brief B-tree 索引
 */
class BTreeIndex : public ComparableIndexBase
{
public:
    BTreeIndex(const std::string & index_name, bool is_unique);

    BTreeIndex(const BTreeIndex &) = delete;

    BTreeIndex(BTreeIndex &&) noexcept = default;

    BTreeIndex & operator=(const BTreeIndex &) = delete;

    BTreeIndex & operator=(BTreeIndex &&) noexcept = default;

    ~BTreeIndex() override = default;

public:
    /** 索引属性接口 */

    /**
     * @brief 设置索引名
     * @param index_name 索引名
     */
    void set_index_name(const std::string & index_name) noexcept;

    /**
     * @brief 设置是否唯一索引
     * @param is_unique 是否唯一索引
     */
    void set_is_unique(bool is_unique) noexcept;

    /**
     * @brief 获取索引名
     * @return 索引名
     */
    const std::string & get_index_name() const;

    /**
     * @brief 获取是否唯一索引
     * @return 是否唯一索引
     */
    bool get_is_unique() const noexcept;

    /** 索引操作接口 */

    /**
     * @brief 插入一个索引项
     * @param key 索引键
     * @param entity_id 实体 ID
     * @return 是否插入成功
     */
    bool insert(std::unique_ptr<ComparableIndexKeyBase> key, std::size_t entity_id) override;

    /**
     * @brief 删除指定索引项
     * @param key 索引键
     * @param entity_id 实体 ID
     * @return 是否删除成功
     */
    bool remove(std::unique_ptr<ComparableIndexKeyBase> key, std::size_t entity_id) override;

    /**
     * @brief 更新指定索引项
     * @param old_key 旧索引键
     * @param new_key 新索引键
     * @param entity_id 实体 ID
     * @return 是否更新成功
     */
    bool update(std::unique_ptr<ComparableIndexKeyBase> old_key, std::unique_ptr<ComparableIndexKeyBase> new_key, std::size_t entity_id) override;

    /** 索引查询接口 */

    /**
     * @brief 查询等于指定索引键的实体 ID 列表
     * @param key 索引键
     * @return 实体 ID 列表
     */
    std::vector<std::size_t> search_equal(std::unique_ptr<ComparableIndexKeyBase> key) const override;

    /**
     * @brief 查询大于指定索引键的实体 ID 列表
     * @param key 索引键
     * @return 实体 ID 列表
     */
    std::vector<std::size_t> search_greater_than(std::unique_ptr<ComparableIndexKeyBase> key) const override;

    /**
     * @brief 查询小于指定索引键的实体 ID 列表
     * @param key 索引键
     * @return 实体 ID 列表
     */
    std::vector<std::size_t> search_less_than(std::unique_ptr<ComparableIndexKeyBase> key) const override;

    /**
     * @brief 查询大于等于指定索引键的实体 ID 列表
     * @param key 索引键
     * @return 实体 ID 列表
     */
    std::vector<std::size_t> search_greater_equal(std::unique_ptr<ComparableIndexKeyBase> key) const override;

    /**
     * @brief 查询小于等于指定索引键的实体 ID 列表
     * @param key 索引键
     * @return 实体 ID 列表
     */
    std::vector<std::size_t> search_less_equal(std::unique_ptr<ComparableIndexKeyBase> key) const override;

    /**
     * @brief 查询指定索引键范围内的实体 ID 列表
     * @param left_key 左边界索引键
     * @param right_key 右边界索引键
     * @return 实体 ID 列表
     */
    std::vector<std::size_t> search_between(std::unique_ptr<ComparableIndexKeyBase> left_key, std::unique_ptr<ComparableIndexKeyBase> right_key) const override;

private:
    std::string index_name_;      // 索引名
    bool is_unique_;              // 是否唯一索引
    std::map<
        std::unique_ptr<ComparableIndexKeyBase>,
        std::vector<std::size_t>,
        ComparableIndexKeyCompare
    > index_map_;                 // 索引项
    // TODO: 将红黑树替换为 B+ 树
};

} // namespace dreamdb
