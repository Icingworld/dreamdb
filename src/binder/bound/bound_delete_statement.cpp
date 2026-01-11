#include "dreamdb/binder/bound/bound_delete_statement.h"

namespace dreamdb
{

BoundDeleteStatement::BoundDeleteStatement()
    : BoundStatement(BoundStatementType::BINDER_BOUND_DELETE_STATEMENT)
    , collection_id(0)
    , where(nullptr)
{
}

} // namespace dreamdb
