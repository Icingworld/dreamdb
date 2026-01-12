#include "dreamdb/binder/bound/bound_statement.h"

namespace dreamdb
{

BoundStatement::BoundStatement(BoundStatementType type) noexcept
    : type_(type)
{
}

BoundStatementType BoundStatement::get_type() const noexcept
{
    return type_;
}

} // namespace dreamdb
