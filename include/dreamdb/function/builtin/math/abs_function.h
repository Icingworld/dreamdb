#pragma once

#include "dreamdb/function/scalar_function.h"

namespace dreamdb
{

/**
 * @brief 绝对值函数
 * @details 绝对值函数用于返回一个数的绝对值
 */
class AbsFunction : public ScalarFunction
{
public:
    explicit AbsFunction();

public:
    LogicalType return_type(const std::vector<LogicalType> & /*argument_types*/) const override;

    std::optional<FieldValue> execute(const std::vector<FieldValue> & args) const override;
};

} // namespace dreamdb
