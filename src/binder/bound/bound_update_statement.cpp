#include "dreamdb/binder/bound/bound_update_statement.h"

namespace dreamdb
{

BoundUpdateStatement::BoundUpdateStatement()
    : BoundStatement(BoundStatementType::BINDER_BOUND_UPDATE_STATEMENT)
    , collection_id(0)
    , update_items()
    , where(nullptr)
{
}

} // namespace dreamdb
