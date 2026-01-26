#pragma once

#include <string>
#include <vector>
#include <optional>

#include "dreamdb/planner/physical/operator/physical_unary_operator.h"
#include "dreamdb/planner/physical/physical_operator.h"

namespace dreamdb::binder::bound
{

class BoundExpression;

} // namespace dreamdb::binder::bound

namespace dreamdb::planner::physical
{

/**
 * @brief 物理投影项
 * @details 用于表示投影的项
 */
struct PhysicalProjectItem
{
    std::unique_ptr<dreamdb::binder::bound::BoundExpression> expression;   // 投影表达式
    std::optional<std::string> alias;                                      // 别名
};

/**
 * @brief 物理投影算子
 * @details 用于投影的算子，对每一行计算投影表达式并输出结果
 */
class PhysicalProject final : public PhysicalUnaryOperator
{
public:
    explicit PhysicalProject(
        std::unique_ptr<PhysicalOperator> child,
        std::vector<PhysicalProjectItem> project_items
    );

    ~PhysicalProject() override = default;

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
     * @brief 评估投影表达式
     * @param context 执行上下文
     * @param input_row 输入行
     * @param output_row 输出行（输出参数）
     */
    void evaluate(ExecutionContext & context, const Row & input_row, Row & output_row) const;

private:
    std::vector<PhysicalProjectItem> project_items_;    // 投影项列表
    Row input_row_;                                     // 临时存储输入行
};

} // namespace dreamdb::planner::physical
