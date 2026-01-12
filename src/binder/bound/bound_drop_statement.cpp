#include "dreamdb/binder/bound/bound_drop_statement.h"

namespace dreamdb
{

BoundDropStatement::BoundDropStatement()
    : BoundStatement(BoundStatementType::BINDER_BOUND_DROP_STATEMENT)
    , if_exists(false)
    , drop_operation(std::monostate{})
{
}

} // namespace dreamdb
