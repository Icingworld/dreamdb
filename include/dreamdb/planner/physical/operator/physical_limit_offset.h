#pragma once

#include <optional>

#include "dreamdb/planner/physical/operator/physical_unary_operator.h"
#include "dreamdb/planner/physical/physical_operator.h"

namespace dreamdb::planner::physical
{

/**
 * @brief 物理限制偏移算子
 * @details 用于限制和偏移结果的算子，实现 LIMIT 和 OFFSET 功能
 */
class PhysicalLimitOffset final : public PhysicalUnaryOperator
{
public:
    explicit PhysicalLimitOffset(
        std::unique_ptr<PhysicalOperator> child,
        std::optional<std::size_t> limit,
        std::optional<std::size_t> offset
    );

    ~PhysicalLimitOffset() override = default;

public:
    /**
     * @brief 打开算子
     * @param context 执行上下文
     */
    void open(dreamdb::executor::ExecutionContext & context) override;

    /**
     * @brief 获取下一行
     * @param context 执行上下文
     * @param row 当前行（输出参数）
     * @return 是否还有下一行
     */
    bool next(dreamdb::executor::ExecutionContext & context, dreamdb::storage::Row & row) override;

    /**
     * @brief 关闭算子
     * @param context 执行上下文
     */
    void close(dreamdb::executor::ExecutionContext & context) override;

private:
    std::optional<std::size_t> limit_;       // 限制行数
    std::optional<std::size_t> offset_;      // 偏移行数
    std::size_t skipped_count_;              // 已跳过的行数
    std::size_t returned_count_;             // 已返回的行数
};

} // namespace dreamdb::planner::physical
