#pragma once

#include <vector>

#include "dreamdb/planner/physical/operator/physical_unary_operator.h"
#include "dreamdb/planner/physical/physical_operator.h"
#include "dreamdb/common/type.h"

namespace dreamdb::binder::bound
{

class BoundExpression;

} // namespace dreamdb::binder::bound

namespace dreamdb::planner::physical
{

/**
 * @brief 物理排序项
 * @details 用于表示排序的项
 */
struct PhysicalSortItem
{
    std::unique_ptr<dreamdb::binder::bound::BoundExpression> expression;   // 排序表达式
    dreamdb::common::Direction direction;                                  // 排序方向
};

/**
 * @brief 物理排序算子
 */
class PhysicalSort final : public PhysicalUnaryOperator
{
public:
    explicit PhysicalSort(
        std::unique_ptr<PhysicalOperator> child,
        std::vector<PhysicalSortItem> sort_items
    );

    ~PhysicalSort() override = default;

public:
    /**
     * @brief 打开算子
     * @param context 执行上下文
     */
    void open(ExecutionContext & context) override;

    /**
     * @brief 获取下一行
     * @param context 执行上下文
     * @param row 当前行（输出参数）
     * @return 是否还有下一行
     */
    bool next(ExecutionContext & context, Row & row) override;

    /**
     * @brief 关闭算子
     * @param context 执行上下文
     */
    void close(ExecutionContext & context) override;

private:
    /**
     * @brief 比较两行
     * @param context 执行上下文
     * @param row1 第一行
     * @param row2 第二行
     * @return 如果 row1 < row2 返回 true，否则返回 false
     */
    bool compare_rows(ExecutionContext & context, const Row & row1, const Row & row2) const;

    /**
     * @brief 评估排序键
     * @param context 执行上下文
     * @param row 输入行
     * @param sort_item_index 排序项索引
     * @return 排序键的值
     */
    dreamdb::FieldValue evaluate_sort_key(
        ExecutionContext & context,
        const Row & row,
        std::size_t sort_item_index
    ) const;

private:
    std::vector<PhysicalSortItem> sort_items_;    // 排序项列表
    std::vector<Row> sorted_rows_;                 // 排序后的行
    std::size_t current_index_;                     // 当前返回的行索引
};

} // namespace dreamdb::planner::physical
