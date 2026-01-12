#include "dreamdb/binder/bound/bound_alter_statement.h"

#include <variant>

namespace dreamdb
{

BoundAlterStatement::BoundAlterStatement()
    : BoundStatement(BoundStatementType::BINDER_BOUND_ALTER_STATEMENT)
    , collection_id(0)
    , alter_operation(std::monostate{})
{
}

} // namespace dreamdb
