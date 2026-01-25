#include "dreamdb/planner/logical/operator/logical_filter.h"

#include <cassert>

#include "dreamdb/binder/bound/expression/expression.h"

namespace dreamdb::planner::logical
{

LogicalFilter::LogicalFilter(std::unique_ptr<dreamdb::binder::bound::BoundExpression> predicate)
    : LogicalOperator(LogicalOperatorType::Filter)
    , predicate_(std::move(predicate))
{
    // 谓词表达式不能为空
    assert(predicate_ != nullptr);

    // 谓词表达式必须是 Boolean 类型结果
    assert(predicate_->logical_type().id == dreamdb::common::LogicalTypeId::Boolean);

    // 注意：子节点是在创建后通过 add_child() 添加的
    // 因此不能在构造函数中检查 child_count()
}

const dreamdb::binder::bound::BoundExpression & LogicalFilter::predicate() const noexcept
{
    return *predicate_;
}

} // namespace dreamdb::planner::logical
