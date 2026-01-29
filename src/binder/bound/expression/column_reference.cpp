#include "dreamdb/binder/bound/expression/column_reference.h"

#include "dreamdb/binder/bound/expression/visitor.h"

namespace dreamdb::binder::bound
{

BoundColumnReferenceExpression::BoundColumnReferenceExpression(
    dreamdb::common::column_oid_t column_id,
    dreamdb::common::LogicalType logical_type
) noexcept
    : BoundExpression(BoundExpressionType::ColumnReference, logical_type)
    , column_id_(column_id)
{
}

dreamdb::common::column_oid_t BoundColumnReferenceExpression::column_id() const noexcept
{
    return column_id_;
}

void BoundColumnReferenceExpression::accept(BoundExpressionVisitor & visitor) const
{
    visitor.visit(*this);
}

std::unique_ptr<BoundExpression> BoundColumnReferenceExpression::clone() const
{
    return std::make_unique<BoundColumnReferenceExpression>(
        column_id(),
        logical_type()
    );
}

} // namespace dreamdb::binder::bound
