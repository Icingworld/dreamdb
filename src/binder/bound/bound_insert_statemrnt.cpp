#include "dreamdb/binder/bound/bound_insert_statement.h"

namespace dreamdb
{

BoundInsertStatement::BoundInsertStatement()
    : BoundStatement(BoundStatementType::BINDER_BOUND_INSERT_STATEMENT)
    , collection_id(0)
    , insert_items()
{
}

} // namespace dreamdb
