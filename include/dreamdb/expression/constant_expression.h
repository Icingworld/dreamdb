#pragma once

#include "dreamdb/expression/expression.h"
#include "dreamdb/schema/field.h"

namespace dreamdb
{
    
/**
 * @brief 常量表达式
 */
class ConstantExpression : public Expression
{
public:
    explicit ConstantExpression(const FieldValue & field_value);

    ConstantExpression(const ConstantExpression &) noexcept = default;

    ConstantExpression(ConstantExpression &&) noexcept = default;

    ConstantExpression &operator=(const ConstantExpression &) noexcept = default;

    ConstantExpression &operator=(ConstantExpression &&) noexcept = default;

    ~ConstantExpression() noexcept = default;

public:
    /**
     * @brief 获取常量值
     * @return 常量值
     */
    const FieldValue & get_field_value() const noexcept;

    /**
     * @brief 判断字段类型是否为指定类型
     * @tparam T 字段类型
     * @return 是否为指定类型
     */
    template <typename T>
    bool is_type() const noexcept;

    /**
     * @brief 获取指定类型指针
     * @tparam T 字段类型
     * @return 指定类型指针
     */
    template <typename T>
    const T * get_field_value_pointer_if_as() const noexcept;

    /**
     * @brief 获取字段值
     * @tparam T 字段类型
     * @return 字段值
     */
    template <typename T>
    const T & get_field_value_as() const;

private:
    FieldValue field_value_;            // 常量值
};

} // namespace dreamdb
