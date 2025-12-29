#pragma once

namespace dreamdb
{

/**
 * @brief 索引键基类
 * @details 索引键基类，用于抽象索引的键，提供比较操作
 */
class IndexKeyBase
{
public:
    virtual ~IndexKeyBase() = default;

public:
    /**
     * @brief 比较两个索引键
     * @param other 另一个索引键
     * @return 比较结果
     */
    virtual int compare(const IndexKeyBase & other) const = 0;

public:
    /**
     * @brief 等于运算符
     * @param other 另一个索引键
     * @return 是否等于
     */
    bool operator==(const IndexKeyBase & other) const;

    /**
     * @brief 不等于运算符
     * @param other 另一个索引键
     * @return 是否不等于
     */
    bool operator!=(const IndexKeyBase & other) const;

    /**
     * @brief 小于运算符
     * @param other 另一个索引键
     * @return 是否小于
     */
    bool operator<(const IndexKeyBase & other) const;

    /**
     * @brief 大于运算符
     * @param other 另一个索引键
     * @return 是否大于
     */
    bool operator>(const IndexKeyBase & other) const;

    /**
     * @brief 小于等于运算符
     * @param other 另一个索引键
     * @return 是否小于等于
     */
    bool operator<=(const IndexKeyBase & other) const;

    /**
     * @brief 大于等于运算符
     * @param other 另一个索引键
     * @return 是否大于等于
     */
    bool operator>=(const IndexKeyBase & other) const;
};

} // namespace dreamdb
