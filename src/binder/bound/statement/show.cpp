#include "dreamdb/binder/bound/statement/show.h"

#include "dreamdb/binder/bound/statement/visitor.h"

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

void BoundShowStatement::accept(BoundStatementVisitor & visitor) const
{
    visitor.visit(*this);
}

} // namespace dreamdb::binder::bound
