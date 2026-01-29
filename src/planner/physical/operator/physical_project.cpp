#include "dreamdb/planner/physical/operator/physical_project.h"

namespace dreamdb::planner::physical
{

PhysicalProject::PhysicalProject(
    std::unique_ptr<PhysicalOperator> child,
    std::vector<PhysicalProjectItem> project_items
)
    : PhysicalUnaryOperator(std::move(child))
    , project_items_(std::move(project_items))
    , input_row_()
{
}

void PhysicalProject::open(dreamdb::executor::ExecutionContext & context)
{
    PhysicalUnaryOperator::open(context);
    input_row_.values.clear();
}

bool PhysicalProject::next(dreamdb::executor::ExecutionContext & context, dreamdb::storage::Row & row)
{
    // 从子算子获取下一行
    if (!child_->next(context, input_row_)) {
        return false;
    }

    // 评估所有投影表达式，生成新的行
    evaluate(context, input_row_, row);
    return true;
}

void PhysicalProject::close(dreamdb::executor::ExecutionContext & context)
{
    PhysicalUnaryOperator::close(context);
    input_row_.values.clear();
}

void PhysicalProject::evaluate(dreamdb::executor::ExecutionContext & context, const dreamdb::storage::Row & input_row, dreamdb::storage::Row & output_row) const
{
    // 清空输出行
    output_row.values.clear();
    output_row.values.reserve(project_items_.size());

    // 对每个投影项计算表达式
    for (const auto & project_item : project_items_) {
        // TODO: 实现表达式求值
        // 需要创建一个表达式求值器（ExpressionEvaluator），它应该：
        // 1. 访问 BoundExpression 树
        // 2. 对于 BoundColumnReferenceExpression，从 input_row 中获取对应列的值
        // 3. 对于 BoundConstantExpression，返回常量值
        // 4. 对于 BoundBinaryExpression，递归求值左右操作数，然后执行运算
        // 5. 对于 BoundUnaryExpression，递归求值操作数，然后执行运算
        // 6. 对于 BoundFunctionCallExpression，求值所有参数，然后调用函数
        // 7. 对于其他表达式类型，类似处理
        //
        // 目前先添加一个占位符，返回 NULL 值
        (void)context;
        (void)input_row;
        (void)project_item;

        // 占位符：返回 NULL 值
        output_row.values.push_back(dreamdb::Null{});
    }
}

} // namespace dreamdb::planner::physical
