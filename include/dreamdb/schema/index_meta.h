#pragma once

#include <string>
#include <vector>

#include "dreamdb/common/type.h"

namespace dreamdb
{

/**
 * @brief 索引元数据
 */
class IndexMeta
{
public:
    IndexMeta();

    IndexMeta(const IndexMeta &) = delete;

    IndexMeta(IndexMeta &&) noexcept = default;

    IndexMeta & operator=(const IndexMeta &) = delete;

    IndexMeta & operator=(IndexMeta &&) noexcept = default;

    ~IndexMeta() = default;

public:
    /**
     * @brief 设置索引名
     * @param index_name 索引名
     */
    void set_index_name(const std::string & index_name);

    /**
     * @brief 设置索引类型
     * @param index_type 索引类型
     */
    void set_index_type(IndexType index_type) noexcept;

    /**
     * @brief 设置是否唯一
     * @param is_unique 是否唯一
     */
    void set_is_unique(bool is_unique) noexcept;

    /**
     * @brief 添加字段索引
     * @param field_index 字段索引
     */
    void add_field_index(std::size_t field_index);

    /**
     * @brief 获取索引名
     * @return 索引名
     */
    const std::string & get_index_name() const noexcept;

    /**
     * @brief 获取索引类型
     * @return 索引类型
     */
    IndexType get_index_type() const noexcept;
    
    /**
     * @brief 获取是否唯一
     * @return 是否唯一
     */
    bool get_is_unique() const noexcept;

    /**
     * @brief 获取字段索引列表
     * @return 字段索引列表
     */
    const std::vector<std::size_t> & get_field_indexes() const noexcept;

    /**
     * @brief 判断是否包含字段索引
     * @param field_index 字段索引
     * @return 是否包含字段索引
     */
    bool has_field_index(std::size_t field_index) const noexcept;

private:
    std::string index_name_;                  // 索引名
    IndexType index_type_;                    // 索引类型
    bool is_unique_;                          // 是否唯一
    std::vector<std::size_t> field_indexes_;  // 字段索引列表
};

} // namespace dreamdb
