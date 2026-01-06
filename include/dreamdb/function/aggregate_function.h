#pragma once

#include "dreamdb/function/function.h"

namespace dreamdb
{

/**
 * @brief 聚合函数基类
 */
class AggregateFunction : public Function
{
public:
    explicit AggregateFunction(std::string func_name, std::vector<LogicalType> arg_types);

    virtual ~AggregateFunction() = default;

public:
    const std::string & name() const noexcept override;

    bool is_aggregate() const noexcept override;

    const std::vector<LogicalType> & argument_types() const noexcept override;

    /**
     * @brief 聚合函数执行接口
     */
    virtual void step(const std::vector<FieldValue> & args) = 0;

    virtual FieldValue finalize() = 0;

    // 聚合函数可以不实现 execute()
    std::optional<FieldValue> execute(const std::vector<FieldValue> & /*args*/) const override;

protected:
    std::string name_;                               // 函数名称
    std::vector<LogicalType> argument_types_;        // 参数类型
};

} // namespace dreamdb
