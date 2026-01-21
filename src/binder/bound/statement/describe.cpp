#include "dreamdb/binder/bound/statement/describe.h"

#include "dreamdb/binder/bound/statement/visitor.h"

namespace dreamdb::binder::bound
{

BoundDescribeStatement::BoundDescribeStatement(dreamdb::common::collection_id_t collection_id) noexcept
    : BoundStatement(BoundStatementType::Describe)
    , collection_id_(collection_id)
{
}

dreamdb::common::collection_id_t BoundDescribeStatement::collection_id() const noexcept
{
    return collection_id_;
}

void BoundDescribeStatement::accept(BoundStatementVisitor & visitor) const
{
    visitor.visit(*this);
}

} // namespace dreamdb::binder::bound
