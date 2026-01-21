#include "dreamdb/binder/bound/statement/alter.h"

#include "dreamdb/binder/bound/statement/visitor.h"

namespace dreamdb::binder::bound
{

BoundAlterStatement::BoundAlterStatement(
    dreamdb::common::collection_id_t collection_id,
    BoundAlterOperation alter_operation
)
    : BoundStatement(BoundStatementType::Alter)
    , collection_id_(collection_id)
    , alter_operation_(std::move(alter_operation))
{
}

dreamdb::common::collection_id_t BoundAlterStatement::collection_id() const noexcept
{
    return collection_id_;
}

const BoundAlterOperation & BoundAlterStatement::alter_operation() const noexcept
{
    return alter_operation_;
}

void BoundAlterStatement::accept(BoundStatementVisitor & visitor) const
{
    visitor.visit(*this);
}

} // namespace dreamdb::binder::bound
