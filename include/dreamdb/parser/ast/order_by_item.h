#pragma once

#include <memory>

#include "dreamdb/common/type.h"

namespace dreamdb
{

class AstExpressionNode;

/**
 * @brief ORDER BY 子句的排序项
 */
class OrderByItem
{
public:
    OrderByItem(std::unique_ptr<AstExpressionNode> expression, Direction order_type = Direction::ASC);

    OrderByItem(const OrderByItem &) = delete;

    OrderByItem(OrderByItem &&) noexcept = default;

    OrderByItem & operator=(const OrderByItem &) = delete;

    OrderByItem & operator=(OrderByItem &&) noexcept = default;

    ~OrderByItem() noexcept;

public:
    /**
    * @brief 获取排序表达式
    * @return 排序表达式
    */
    const AstExpressionNode & get_expression() const noexcept;

    /**
    * @brief 获取排序类型
    * @return 排序类型
    */
    Direction get_order_type() const noexcept;

    /**
     * @brief 是否存在排序表达式
     * @return 是否存在排序表达式
     */
    bool has_expression() const noexcept;

private:
    std::unique_ptr<AstExpressionNode> expression_;  // 排序表达式
    Direction order_type_;                           // 排序类型
};


} // namespace dreamdb
