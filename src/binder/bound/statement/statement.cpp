#include "dreamdb/binder/bound/statement/statement.h"

namespace dreamdb::binder::bound
{

BoundStatement::BoundStatement(BoundStatementType type) noexcept
    : type_(type)
{
}

BoundStatementType BoundStatement::type() const noexcept
{
    return type_;
}

} // namespace dreamdb::binder::bound
