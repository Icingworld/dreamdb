#include "dreamdb/binder/bound/statement/use.h"

namespace dreamdb::binder::bound
{

BoundUseStatement::BoundUseStatement(dreamdb::common::database_id_t database_id) noexcept
    : BoundStatement(BoundStatementType::Use)
    , database_id_(database_id)
{
}

dreamdb::common::database_id_t BoundUseStatement::database_id() const noexcept
{
    return database_id_;
}

} // namespace dreamdb::binder::bound
