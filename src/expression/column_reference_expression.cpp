#include "dreamdb/expression/column_reference_expression.h"

namespace dreamdb
{

ColumnReferenceExpression::ColumnReferenceExpression(std::size_t field_index) noexcept
    : Expression(ExpressionType::EXPRESSION_COLUMN_REFERENCE)
    , field_index_(field_index)
{
}

std::size_t ColumnReferenceExpression::get_field_index() const noexcept
{
    return field_index_;
}

} // namespace dreamdb
