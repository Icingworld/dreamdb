#pragma once

#include <cstdint>
#include <memory>

namespace dreamdb::planner::physical
{

class PhysicalOperator;

/**
 * @brief 结果类型
 */
enum class ResultType : std::uint8_t
{
    Query,       // 查询
    Mutation     // 修改
};

/**
 * @brief 物理计划
 */
class PhysicalPlan
{
public:
    ResultType result_type() const;

    PhysicalOperator & root();

private:
    ResultType result_type_;                     // 结果类型
    std::unique_ptr<PhysicalOperator> root_;     // 根算子
};

} // namespace dreamdb::planner::physical
