#pragma once

#include <cstddef>
#include <vector>
#include <memory>

#include "dreamdb/index/index_key_base.h"
#include "dreamdb/schema/field.h"

namespace dreamdb
{

class Entity;

/**
 * @brief 索引键具体实现
 * @details 同时实现 ComparableIndexKeyBase 和 HashableIndexKeyBase
 *          使用 std::vector<FieldValue> 存储复合键，支持任意字段组合
 */
class IndexKey : public ComparableIndexKeyBase, public HashableIndexKeyBase
{
public:
    explicit IndexKey(const std::vector<FieldValue> & values);

    IndexKey(const IndexKey & other) = default;

    IndexKey(IndexKey && other) noexcept = default;

    IndexKey& operator=(const IndexKey & other) = default;

    IndexKey& operator=(IndexKey && other) noexcept = default;

    ~IndexKey() noexcept override = default;

public:
    /** 属性接口 */
    const std::vector<FieldValue> & get_values() const noexcept;

public:
    /** ComparableIndexKeyBase 要求实现 */

    /**
     * @brief 比较两个索引键
     * @param other 另一个索引键
     * @return 比较结果
     */
    int compare(const ComparableIndexKeyBase& other) const override;

    /** HashableIndexKeyBase 要求实现 */

    /**
     * @brief 哈希值
     * @return 哈希值
     */
    std::size_t hash() const override;

    /**
     * @brief 等于运算符
     * @param other 另一个索引键
     * @return 是否等于
     */
    bool equals(const HashableIndexKeyBase& other) const override;

private:
    std::vector<FieldValue> values_;     // 字段值列表
};

} // namespace dreamdb