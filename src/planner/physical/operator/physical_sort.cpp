#include "dreamdb/planner/physical/operator/physical_sort.h"

#include <algorithm>

namespace dreamdb::planner::physical
{

PhysicalSort::PhysicalSort(
    std::unique_ptr<PhysicalOperator> child,
    std::vector<PhysicalSortItem> sort_items
)
    : PhysicalUnaryOperator(std::move(child)),
      sort_items_(std::move(sort_items)),
      current_index_(0)
{
}

void PhysicalSort::open(ExecutionContext & context)
{
    PhysicalUnaryOperator::open(context);

    // 收集所有行
    sorted_rows_.clear();
    Row row;
    while (child_->next(context, row)) {
        sorted_rows_.push_back(row);
    }

    // 对行进行排序
    // 使用 lambda 表达式作为比较函数
    std::sort(
        sorted_rows_.begin(),
        sorted_rows_.end(),
        [this, &context](const Row & row1, const Row & row2) {
            return compare_rows(context, row1, row2);
        }
    );

    // 重置索引
    current_index_ = 0;
}

bool PhysicalSort::next(ExecutionContext & context, Row & row)
{
    (void)context;

    // 检查是否还有更多行
    if (current_index_ >= sorted_rows_.size()) {
        return false;
    }

    // 返回当前行并递增索引
    row = sorted_rows_[current_index_];
    ++current_index_;
    return true;
}

void PhysicalSort::close(ExecutionContext & context)
{
    PhysicalUnaryOperator::close(context);
    sorted_rows_.clear();
    current_index_ = 0;
}

bool PhysicalSort::compare_rows(ExecutionContext & context, const Row & row1, const Row & row2) const
{
    // 按照所有排序项进行比较
    for (std::size_t i = 0; i < sort_items_.size(); ++i) {
        const auto & sort_item = sort_items_[i];

        // 评估两个行的排序键
        auto key1 = evaluate_sort_key(context, row1, i);
        auto key2 = evaluate_sort_key(context, row2, i);

        // 比较排序键
        int comparison = 0;

        // TODO: 实现 FieldValue 的比较逻辑
        // 需要根据 FieldValue 的类型进行比较：
        // 1. 如果两个值都是 NULL，则相等
        // 2. 如果一个是 NULL，另一个不是，NULL 值应该排在前面（或后面，取决于 SQL 标准）
        // 3. 对于数值类型，直接比较
        // 4. 对于字符串类型，使用字符串比较
        // 5. 对于布尔类型，false < true
        // 6. 对于其他类型，类似处理
        //
        // 目前先使用简单的占位符比较
        (void)key1;
        (void)key2;
        comparison = 0;  // 占位符：暂时认为相等

        // 如果两个键不相等，根据排序方向返回结果
        if (comparison != 0) {
            if (sort_item.direction == dreamdb::common::Direction::ASC) {
                return comparison < 0;
            } else {
                return comparison > 0;
            }
        }
        // 如果相等，继续比较下一个排序键
    }

    // 所有排序键都相等
    return false;
}

dreamdb::FieldValue PhysicalSort::evaluate_sort_key(
    ExecutionContext & context,
    const Row & row,
    std::size_t sort_item_index
) const
{
    // TODO: 实现表达式求值
    // 需要创建一个表达式求值器（ExpressionEvaluator），它应该：
    // 1. 访问 BoundExpression 树
    // 2. 对于 BoundColumnReferenceExpression，从 row 中获取对应列的值
    // 3. 对于 BoundConstantExpression，返回常量值
    // 4. 对于 BoundBinaryExpression，递归求值左右操作数，然后执行运算
    // 5. 对于 BoundUnaryExpression，递归求值操作数，然后执行运算
    // 6. 对于 BoundFunctionCallExpression，求值所有参数，然后调用函数
    // 7. 对于其他表达式类型，类似处理
    //
    // 目前先返回 NULL 值作为占位符
    (void)context;
    (void)row;
    (void)sort_item_index;

    return dreamdb::Null{};
}

} // namespace dreamdb::planner::physical
