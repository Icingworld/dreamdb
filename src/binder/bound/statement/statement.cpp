#include "dreamdb/binder/bound/statement/statement.h"

namespace dreamdb::binder::bound
{

BoundStatement::BoundStatement(BoundStatementType type) noexcept
    : statement_type_(type)
{
}

BoundStatementType BoundStatement::statement_type() const noexcept
{
    return statement_type_;
}

} // namespace dreamdb::binder::bound
