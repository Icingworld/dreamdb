#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <optional>
#include <string>

#include "dreamdb/common/logical_type.h"

namespace dreamdb::planner::logical
{

/**
 * @brief 逻辑算子类型
 */
enum class LogicalOperatorType : std::uint8_t
{
    /** DQL 语句算子 */
    Scan,                   // 扫描集合
    Filter,                 // 过滤（WHERE 子句）
    Project,                // 投影（SELECT 列表）
    Aggregate,              // 聚合（聚合函数，如 COUNT、SUM、AVG 等）
    Sort,                   // 排序（ORDER BY 子句）
    LimitOffset,            // 限制和偏移（LIMIT 和 OFFSET 子句）

    /** DML 语句算子 */
    Update,                 // UPDATE 操作
    Delete                  // DELETE 操作
};

/**
 * @brief 逻辑算子输出模式
 */
struct LogicalOutputSchema
{
    std::vector<dreamdb::common::LogicalType> types;
    std::optional<std::size_t> row_id_index; // 若存在表示输入/输出含 row_id
    std::vector<std::string> names;          // 可选：用于结果列名（可后置）
};

/**
 * @brief 逻辑算子基类
 * @details 所有逻辑算子的基类，提供通用的树形结构支持
 */
class LogicalOperator
{
public:
    explicit LogicalOperator(LogicalOperatorType operator_type) noexcept;

    virtual ~LogicalOperator() noexcept = default;

public:
    /**
     * @brief 获取逻辑算子类型
     * @return 逻辑算子类型
     */
    LogicalOperatorType operator_type() const noexcept;

    /**
     * @brief 获取逻辑算子输出模式
     * @return 逻辑算子输出模式
     */
    const LogicalOutputSchema & output_schema() const noexcept;

    /**
     * @brief 获取子节点数量
     * @return 子节点数量
     */
    std::size_t child_count() const noexcept;

    /**
     * @brief 获取指定索引的子节点
     * @param index 子节点索引
     * @return 子节点
     */
    const LogicalOperator & child_at(std::size_t index) const noexcept;

protected:
    LogicalOperatorType operator_type_;                        // 逻辑算子类型
    LogicalOutputSchema output_schema_;                        // 逻辑算子输出模式
    std::vector<std::unique_ptr<LogicalOperator>> children_;   // 子节点
};

} // namespace dreamdb::planner::logical
