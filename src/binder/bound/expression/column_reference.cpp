#include "dreamdb/binder/bound/expression/column_reference.h"

namespace dreamdb::binder::bound
{

ColumnReferenceExpression::ColumnReferenceExpression(
    dreamdb::common::column_id_t column_id,
    dreamdb::common::LogicalType logical_type
) noexcept
    : BoundExpression(BoundExpressionType::ColumnReference, logical_type)
    , column_id_(column_id)
{
}

dreamdb::common::column_id_t ColumnReferenceExpression::column_id() const noexcept
{
    return column_id_;
}

} // namespace dreamdb::binder::bound
