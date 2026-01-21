#include "dreamdb/binder/bound/statement/drop.h"

#include "dreamdb/binder/bound/statement/visitor.h"

namespace dreamdb::binder::bound
{

BoundDropStatement::BoundDropStatement(BoundDropOperation operation, bool if_exists)
    : BoundStatement(BoundStatementType::Drop)
    , if_exists_(if_exists)
    , operation_(operation)
{
}

bool BoundDropStatement::if_exists() const noexcept
{
    return if_exists_;
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
