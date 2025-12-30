#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "dreamdb/index/index_base.h"
#include "dreamdb/index/index_key_base.h"

namespace dreamdb
{

/**
 * @brief HashableIndexKeyBase 的哈希函数
 */
struct HashableIndexKeyHash
{
    std::size_t operator()(const std::unique_ptr<HashableIndexKeyBase> & key) const
    {
        if (key == nullptr) {
            return 0;
        }
        return key->hash();
    }
};

/**
 * @brief HashableIndexKeyBase 的相等比较函数
 */
struct HashableIndexKeyEqual
{
    bool operator()(const std::unique_ptr<HashableIndexKeyBase> & lhs, const std::unique_ptr<HashableIndexKeyBase> & rhs) const
    {
        if (lhs == rhs) {
            return true;
        }
        if (lhs == nullptr || rhs == nullptr) {
            return false;
        }
        return lhs->equals(*rhs);
    }
};

/**
 * @brief 哈希索引
 */
class HashIndex : public HashableIndexBase
{
public:
    HashIndex(const std::string & index_name, bool is_unique);

    HashIndex(const HashIndex &) = delete;

    HashIndex(HashIndex &&) noexcept = default;

    HashIndex & operator=(const HashIndex &) = delete;

    HashIndex & operator=(HashIndex &&) noexcept = default;

    ~HashIndex() override = default;

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
    bool insert(std::unique_ptr<HashableIndexKeyBase> key, std::size_t entity_id) override;

    /**
     * @brief 删除指定索引项
     * @param key 索引键
     * @param entity_id 实体 ID
     * @return 是否删除成功
     */
    bool remove(std::unique_ptr<HashableIndexKeyBase> key, std::size_t entity_id) override;

    /**
     * @brief 更新指定索引项
     * @param old_key 旧索引键
     * @param new_key 新索引键
     * @param entity_id 实体 ID
     * @return 是否更新成功
     */
    bool update(std::unique_ptr<HashableIndexKeyBase> old_key, std::unique_ptr<HashableIndexKeyBase> new_key, std::size_t entity_id) override;

    /** 索引查询接口 */

    /**
     * @brief 查询等于指定索引键的实体 ID 列表
     * @param key 索引键
     * @return 实体 ID 列表
     */
    std::vector<std::size_t> search_equal(std::unique_ptr<HashableIndexKeyBase> key) const override;

private:
    std::string index_name_;      // 索引名
    bool is_unique_;              // 是否唯一索引
    std::unordered_map<
        std::unique_ptr<HashableIndexKeyBase>,
        std::vector<std::size_t>,
        HashableIndexKeyHash,
        HashableIndexKeyEqual
    > index_map_;                 // 索引项
};

} // namespace dreamdb
