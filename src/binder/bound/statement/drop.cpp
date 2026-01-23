#include "dreamdb/binder/bound/statement/drop.h"

#include "dreamdb/binder/bound/statement/visitor.h"

namespace dreamdb::binder::bound
{

BoundDropStatement::BoundDropStatement(BoundDropOperation operation)
    : BoundStatement(BoundStatementType::Drop)
    , operation_(operation)
{
}

const BoundDropOperation & BoundDropStatement::operation() const noexcept
{
    return operation_;
}

void BoundDropStatement::accept(BoundStatementVisitor & visitor) const
{
    visitor.visit(*this);
}

} // namespace dreamdb::binder::bound
