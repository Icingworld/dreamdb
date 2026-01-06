#pragma once

#include "dreamdb/function/function.h"

namespace dreamdb
{

class ScalarFunction : public Function
{
public:
    explicit ScalarFunction(const std::string & name, const std::vector<LogicalType> & argument_types) noexcept;

    ~ScalarFunction() noexcept override = default;

public:
    const std::string & name() const noexcept override;

    bool is_aggregate() const noexcept override;

    const std::vector<LogicalType> & argument_types() const noexcept override;

protected:
    std::string name_;                               // 函数名称
    std::vector<LogicalType> argument_types_;        // 参数类型
};

} // namespace dreamdb
