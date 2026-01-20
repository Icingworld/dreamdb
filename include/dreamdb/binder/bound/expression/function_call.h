#pragma once

#include <string>
#include <vector>
#include <memory>

#include "dreamdb/binder/bound/expression/expression.h"
#include "dreamdb/common/logical_type.h"

namespace dreamdb::binder::bound
{

/**
 * @brief 绑定后的函数调用表达式
 */
class BoundFunctionCallExpression final : public BoundExpression
{
public:
    explicit BoundFunctionCallExpression(
        std::string function_name,
        std::vector<std::unique_ptr<BoundExpression>> arguments,
        dreamdb::common::LogicalType logical_type,
        bool is_aggregate = false
    );

    ~BoundFunctionCallExpression() noexcept override = default;

public:
    /**
     * @brief 获取函数名称
     * @return 函数名称
     */
    const std::string & function_name() const noexcept;

    /**
     * @brief 获取参数数量
     * @return 参数数量
     */
    std::size_t argument_count() const noexcept;

    /**
     * @brief 获取指定索引的参数
     * @param index 参数索引
     * @return 参数表达式
     */
    const BoundExpression & argument_at(std::size_t index) const noexcept;

    /**
     * @brief 是否为聚合函数
     * @return 是否为聚合函数
     */
    bool is_aggregate() const noexcept;

private:
    std::string function_name_;                                      // 函数名称
    std::vector<std::unique_ptr<BoundExpression>> arguments_;       // 参数列表
    bool is_aggregate_;                                              // 是否为聚合函数
};

} // namespace dreamdb::binder::bound
