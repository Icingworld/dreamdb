#include "dreamdb/binder/bound/bound_describe_statement.h"

namespace dreamdb
{

BoundDescribeStatement::BoundDescribeStatement()
    : BoundStatement(BoundStatementType::BINDER_BOUND_DESCRIBE_STATEMENT)
    , collection_id(0)
{
}

} // namespace dreamdb
