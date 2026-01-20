#pragma once

#include <variant>

#include "dreamdb/binder/bound/expression/expression.h"
#include "dreamdb/common/logical_type.h"
#include "dreamdb/schema/field.h"

namespace dreamdb::binder::bound
{

/**
 * @brief 绑定后的常量表达式
 */
class BoundConstantExpression final : public BoundExpression
{
public:
    explicit BoundConstantExpression(
        dreamdb::FieldValue field_value,
        dreamdb::common::LogicalType logical_type
    );

    ~BoundConstantExpression() noexcept override = default;

public:
    /**
     * @brief 获取常量值
     * @return 常量值
     */
    const dreamdb::FieldValue & value() const noexcept;

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
     * @return 指定类型指针，如果类型不匹配返回 nullptr
     */
    template <typename T>
    const T * value_pointer_if_as() const noexcept;

    /**
     * @brief 获取字段值
     * @tparam T 字段类型
     * @return 字段值
     * @throws std::bad_variant_access 如果类型不匹配
     */
    template <typename T>
    const T & value_as() const;

private:
    dreamdb::FieldValue field_value_;            // 常量值
};

// 模板实现
template <typename T>
bool BoundConstantExpression::is_type() const noexcept
{
    return std::holds_alternative<T>(field_value_);
}

template <typename T>
const T * BoundConstantExpression::value_pointer_if_as() const noexcept
{
    if (std::holds_alternative<T>(field_value_)) {
        return &std::get<T>(field_value_);
    }
    return nullptr;
}

template <typename T>
const T & BoundConstantExpression::value_as() const
{
    return std::get<T>(field_value_);
}

} // namespace dreamdb::binder::bound
