#include "dreamdb/binder/bound/bound_show_statement.h"

namespace dreamdb
{

BoundShowStatement::BoundShowStatement()
    : BoundStatement(BoundStatementType::BINDER_BOUND_SHOW_STATEMENT)
    , show_operation(std::monostate{})
{
}

} // namespace dreamdb
