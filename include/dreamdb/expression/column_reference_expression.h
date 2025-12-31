#pragma once

#include "dreamdb/expression/expression.h"

namespace dreamdb
{

/**
 * @brief 字段引用表达式
 */
class ColumnReferenceExpression : public Expression
{
public:
    explicit ColumnReferenceExpression(std::size_t field_index) noexcept;

    ColumnReferenceExpression(const ColumnReferenceExpression &) noexcept = default;

    ColumnReferenceExpression(ColumnReferenceExpression &&) noexcept = default;

    ColumnReferenceExpression &operator=(const ColumnReferenceExpression &) noexcept = default;

    ColumnReferenceExpression &operator=(ColumnReferenceExpression &&) noexcept = default;

    ~ColumnReferenceExpression() noexcept = default;

public:
    /**
     * @brief 获取字段索引
     * @return 字段索引
     */
    std::size_t get_field_index() const noexcept;

private:
    std::size_t field_index_;            // 字段索引
};

} // namespace dreamdb
