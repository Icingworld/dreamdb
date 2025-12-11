#pragma once

#include <optional>
#include <memory>

#include "dreamdb/query/condition.h"
#include "dreamdb/query/order.h"
#include "dreamdb/query/limit.h"

namespace dreamdb
{

/**
 * @brief 查询类
 * @details 用于表示完整的查询请求，包含条件、排序和限制
 */
class Query
{
public:
    Query() = default;

    Query(const Query & other) = default;

    Query(Query && other) noexcept = default;

    Query & operator=(const Query & other) = default;

    Query & operator=(Query && other) noexcept = default;

    ~Query() = default;

public:
    /**
     * @brief 设置查询条件（WHERE 子句）
     * @param condition 查询条件
     * @return 当前 Query 对象的引用，支持链式调用
     */
    Query & where(const Condition & condition);

    /**
     * @brief 设置排序规则（ORDER BY 子句）
     * @param order 排序规则
     * @return 当前 Query 对象的引用，支持链式调用
     */
    Query & order_by(const Order & order);

    /**
     * @brief 设置限制数量（LIMIT 子句）
     * @param limit 限制数量
     * @return 当前 Query 对象的引用，支持链式调用
     */
    Query & limit(const Limit & limit);

public:
    /**
     * @brief 获取查询条件
     * @return 查询条件，如果没有设置返回 std::nullopt
     */
    const std::optional<Condition> & get_condition() const noexcept;

    /**
     * @brief 获取排序规则
     * @return 排序规则，如果没有设置返回 std::nullopt
     */
    const std::optional<Order> & get_order() const noexcept;

    /**
     * @brief 获取限制数量
     * @return 限制数量，如果没有设置返回 std::nullopt
     */
    const std::optional<Limit> & get_limit() const noexcept;

    /**
     * @brief 检查是否有查询条件
     * @return 如果有查询条件返回 true
     */
    bool has_condition() const noexcept;

    /**
     * @brief 检查是否有排序规则
     * @return 如果有排序规则返回 true
     */
    bool has_order() const noexcept;

    /**
     * @brief 检查是否有限制数量
     * @return 如果有限制数量返回 true
     */
    bool has_limit() const noexcept;

    /**
     * @brief 检查查询是否为空（没有任何条件、排序或限制）
     * @return 如果查询为空返回 true
     */
    bool is_empty() const noexcept;

    /**
     * @brief 清空所有查询条件
     */
    void clear();

private:
    std::optional<Condition> condition_;    // 查询条件（WHERE 子句）
    std::optional<Order> order_;            // 排序规则（ORDER BY 子句）
    std::optional<Limit> limit_;            // 限制数量（LIMIT 子句）
};

} // namespace dreamdb
