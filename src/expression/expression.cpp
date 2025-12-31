#include "dreamdb/expression/expression.h"

namespace dreamdb
{

Expression::Expression(ExpressionType type) noexcept
    : type_(type)
{
}

ExpressionType Expression::get_type() const noexcept
{
    return type_;
}

} // namespace dreamdb
