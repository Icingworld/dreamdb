#pragma once

#include <string>
#include <vector>
#include <optional>

#include "dreamdb/schema/field.h"
#include "dreamdb/catalog/logical_type.h"

namespace dreamdb
{

/**
 * @brief 函数接口
 * @details 函数接口用于定义函数的基本信息和执行逻辑
 */
class Function
{
public:
    virtual ~Function() noexcept = default;

public:
    /**
     * @brief 函数名称
     * @return 函数名称
     */
    virtual const std::string & name() const noexcept = 0;

    /**
     * @brief 是否为聚合函数
     * @return 是否为聚合函数
     */
    virtual bool is_aggregate() const noexcept = 0;

    /**
     * @brief 参数类型
     * @return 参数类型
     * @details 用于 Binder 进行参数类型检查
     */
    virtual const std::vector<LogicalType> & argument_types() const noexcept = 0;

    /**
     * @brief 返回值类型
     * @param argument_types 参数类型
     * @return 返回值类型
     * @details 用于 Binder 进行返回值类型检查
     */
    virtual LogicalType return_type(const std::vector<LogicalType> & argument_types) const = 0;

    /**
     * @brief 执行函数
     * @param args 参数值
     * @return 执行结果
     * @details 标量函数直接执行，聚合函数重载后返回 std::nullopt 即可
     */
    virtual std::optional<FieldValue> execute(const std::vector<FieldValue> & args) const = 0;
};

} // namespace dreamdb
