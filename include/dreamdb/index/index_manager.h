#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>
#include <memory>

#include "dreamdb/index/index_base.h"

namespace dreamdb
{

/**
 * @brief 索引管理器
 */
class IndexManager
{
public:
    IndexManager() = default;

    IndexManager(const IndexManager &) = delete;

    IndexManager(IndexManager &&) noexcept = default;

    IndexManager & operator=(const IndexManager &) = delete;

    IndexManager & operator=(IndexManager &&) noexcept = default;

    ~IndexManager() = default;

public:
    /** 索引生命周期管理 */

    /**
     * @brief 注册一个索引
     * @param index_name 索引名
     * @param index 索引
     * @return 是否注册成功
     */
    bool register_index(const std::string & index_name, std::unique_ptr<IndexBase> index) noexcept;

    /**
     * @brief 注销一个索引
     * @param index_name 索引名
     * @return 是否注销成功
     */
    bool unregister_index(const std::string & index_name) noexcept;

    /**
     * @brief 判断是否存在一个索引
     * @param index_name 索引名
     * @return 是否存在
     */
    bool has_index(const std::string & index_name) const noexcept;

    /** ComparableIndex 索引调度接口 */

    /**
     * @brief 插入一个 ComparableIndex 索引项
     * @param index_name 索引名
     * @param key 索引键
     * @param entity_id 实体 ID
     * @return 是否插入成功
     */
    bool insert_comparable_index(const std::string & index_name, std::unique_ptr<ComparableIndexKeyBase> key, std::size_t entity_id) noexcept;

    /**
     * @brief 删除一个 ComparableIndex 索引项
     * @param index_name 索引名
     * @param key 索引键
     * @param entity_id 实体 ID
     * @return 是否删除成功
     */
    bool remove_comparable_index(const std::string & index_name, std::unique_ptr<ComparableIndexKeyBase> key, std::size_t entity_id) noexcept;

    /**
     * @brief 更新一个 ComparableIndex 索引项
     * @param index_name 索引名
     * @param old_key 旧索引键
     * @param new_key 新索引键
     * @param entity_id 实体 ID
     * @return 是否更新成功
     */
    bool update_comparable_index(const std::string & index_name, std::unique_ptr<ComparableIndexKeyBase> old_key, std::unique_ptr<ComparableIndexKeyBase> new_key, std::size_t entity_id) noexcept;

    /**
     * @brief 搜索等于一个 ComparableIndex 索引项
     * @param index_name 索引名
     * @param key 索引键
     * @return 是否搜索成功
     */
    std::vector<std::size_t> search_comparable_index_equal(const std::string & index_name, std::unique_ptr<ComparableIndexKeyBase> key) const noexcept;

    /**
     * @brief 搜索大于一个 ComparableIndex 索引项
     * @param index_name 索引名
     * @param key 索引键
     * @return 是否搜索成功
     */
    std::vector<std::size_t> search_comparable_index_greater_than(const std::string & index_name, std::unique_ptr<ComparableIndexKeyBase> key) const noexcept;

    /**
     * @brief 搜索小于一个 ComparableIndex 索引项
     * @param index_name 索引名
     * @param key 索引键
     * @return 是否搜索成功
     */
    std::vector<std::size_t> search_comparable_index_less_than(const std::string & index_name, std::unique_ptr<ComparableIndexKeyBase> key) const noexcept;

    /**
     * @brief 搜索大于等于一个 ComparableIndex 索引项
     * @param index_name 索引名
     * @param key 索引键
     * @return 是否搜索成功
     */
    std::vector<std::size_t> search_comparable_index_greater_equal(const std::string & index_name, std::unique_ptr<ComparableIndexKeyBase> key) const noexcept;

    /**
     * @brief 搜索小于等于一个 ComparableIndex 索引项
     * @param index_name 索引名
     * @param key 索引键
     * @return 是否搜索成功
     */
    std::vector<std::size_t> search_comparable_index_less_equal(const std::string & index_name, std::unique_ptr<ComparableIndexKeyBase> key) const noexcept;

    /**
     * @brief 搜索介于两个 ComparableIndex 索引项之间
     * @param index_name 索引名
     * @param left_key 左索引键
     * @param right_key 右索引键
     * @return 是否搜索成功
     */
    std::vector<std::size_t> search_comparable_index_between(const std::string & index_name, std::unique_ptr<ComparableIndexKeyBase> left_key, std::unique_ptr<ComparableIndexKeyBase> right_key) const noexcept;

    /** HashableIndex 索引调度接口 */

    /**
     * @brief 插入一个 HashableIndex 索引项
     * @param index_name 索引名
     * @param key 索引键
     * @param entity_id 实体 ID
     * @return 是否插入成功
     */
    bool insert_hashable_index(const std::string & index_name, std::unique_ptr<HashableIndexKeyBase> key, std::size_t entity_id) noexcept;

    /**
     * @brief 删除一个 HashableIndex 索引项
     * @param index_name 索引名
     * @param key 索引键
     * @param entity_id 实体 ID
     * @return 是否删除成功
     */
    bool remove_hashable_index(const std::string & index_name, std::unique_ptr<HashableIndexKeyBase> key, std::size_t entity_id) noexcept;

    /**
     * @brief 更新一个 HashableIndex 索引项
     * @param index_name 索引名
     * @param old_key 旧索引键
     * @param new_key 新索引键
     * @param entity_id 实体 ID
     * @return 是否更新成功
     */
    bool update_hashable_index(const std::string & index_name, std::unique_ptr<HashableIndexKeyBase> old_key, std::unique_ptr<HashableIndexKeyBase> new_key, std::size_t entity_id) noexcept;

    /**
     * @brief 搜索等于一个 HashableIndex 索引项
     * @param index_name 索引名
     * @param key 索引键
     * @return 是否搜索成功
     */
    std::vector<std::size_t> search_hashable_index_equal(const std::string & index_name, std::unique_ptr<HashableIndexKeyBase> key) const noexcept;

private:
    std::unordered_map<std::string, std::unique_ptr<IndexBase>> indexes_;  // 索引名到索引的映射
};

} // namespace dreamdb
