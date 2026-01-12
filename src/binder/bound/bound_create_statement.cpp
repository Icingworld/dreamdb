#include "dreamdb/binder/bound/bound_create_statement.h"

namespace dreamdb
{

BoundCreateStatement::BoundCreateStatement()
    : BoundStatement(BoundStatementType::BINDER_BOUND_CREATE_STATEMENT)
    , if_not_exists(false)
    , create_operation(std::monostate{})
{
}

} // namespace dreamdb
