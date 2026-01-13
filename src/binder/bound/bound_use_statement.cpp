#include "dreamdb/binder/bound/bound_use_statement.h"

namespace dreamdb
{

BoundUseStatement::BoundUseStatement()
    : BoundStatement(BoundStatementType::BINDER_BOUND_USE_STATEMENT)
    , database_id(0)
{
}

} // namespace dreamdb
