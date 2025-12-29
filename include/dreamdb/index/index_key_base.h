#pragma once

#include <cstddef>

namespace dreamdb
{

/**
 * @brief 可比较的索引键基类
 * @details 用于 B-tree 等需要排序的索引，提供比较操作
 */
class ComparableIndexKeyBase
{
public:
    virtual ~ComparableIndexKeyBase() = default;

public:
    /**
     * @brief 比较两个索引键
     * @param other 另一个索引键
     * @return 比较结果：负数表示小于，0表示等于，正数表示大于
     */
    virtual int compare(const ComparableIndexKeyBase & other) const = 0;

public:
    /**
     * @brief 等于运算符
     * @param other 另一个索引键
     * @return 是否等于
     */
    bool operator==(const ComparableIndexKeyBase & other) const;

    /**
     * @brief 不等于运算符
     * @param other 另一个索引键
     * @return 是否不等于
     */
    bool operator!=(const ComparableIndexKeyBase & other) const;

    /**
     * @brief 小于运算符
     * @param other 另一个索引键
     * @return 是否小于
     */
    bool operator<(const ComparableIndexKeyBase & other) const;

    /**
     * @brief 大于运算符
     * @param other 另一个索引键
     * @return 是否大于
     */
    bool operator>(const ComparableIndexKeyBase & other) const;

    /**
     * @brief 小于等于运算符
     * @param other 另一个索引键
     * @return 是否小于等于
     */
    bool operator<=(const ComparableIndexKeyBase & other) const;

    /**
     * @brief 大于等于运算符
     * @param other 另一个索引键
     * @return 是否大于等于
     */
    bool operator>=(const ComparableIndexKeyBase & other) const;
};

/**
 * @brief 可哈希的索引键基类
 * @details 用于 Hash 索引，提供哈希和相等比较操作
 */
class HashableIndexKeyBase
{
public:
    virtual ~HashableIndexKeyBase() = default;

public:
    /**
     * @brief 计算索引键的哈希值
     * @return 哈希值
     */
    virtual std::size_t hash() const = 0;

    /**
     * @brief 判断两个索引键是否相等
     * @param other 另一个索引键
     * @return 是否相等
     */
    virtual bool equals(const HashableIndexKeyBase & other) const = 0;

public:
    /**
     * @brief 等于运算符
     * @param other 另一个索引键
     * @return 是否等于
     */
    bool operator==(const HashableIndexKeyBase & other) const;
};

} // namespace dreamdb
