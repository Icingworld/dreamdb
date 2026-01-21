#include "dreamdb/binder/bound/statement/delete.h"

#include <cassert>

#include "dreamdb/binder/bound/statement/visitor.h"

namespace dreamdb::binder::bound
{

BoundDeleteStatement::BoundDeleteStatement(
    dreamdb::common::collection_id_t collection_id,
    std::unique_ptr<BoundExpression> where
)
    : BoundStatement(BoundStatementType::Delete)
    , collection_id_(collection_id)
    , where_(std::move(where))
{
}

void BoundDeleteStatement::accept(BoundStatementVisitor & visitor) const
{
    visitor.visit(*this);
}

dreamdb::common::collection_id_t BoundDeleteStatement::collection_id() const noexcept
{
    return collection_id_;
}

bool BoundDeleteStatement::has_where() const noexcept
{
    return where_ != nullptr;
}

const BoundExpression * BoundDeleteStatement::where() const noexcept
{
    return where_.get();
}

const BoundExpression & BoundDeleteStatement::where_ref() const noexcept
{
    // WHERE 子句必须存在
    assert(where_ != nullptr);

    return *where_;
}

} // namespace dreamdb::binder::bound
