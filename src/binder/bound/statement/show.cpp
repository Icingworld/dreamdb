#include "dreamdb/binder/bound/statement/show.h"

namespace dreamdb::binder::bound
{

BoundShowStatement::BoundShowStatement(BoundShowOperation operation)
    : BoundStatement(BoundStatementType::Show)
    , operation_(operation)
{
}

const BoundShowOperation & BoundShowStatement::operation() const noexcept
{
    return operation_;
}

} // namespace dreamdb::binder::bound
