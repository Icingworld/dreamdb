#include "dreamdb/binder/bound/statement/insert.h"

#include <cassert>

#include "dreamdb/binder/bound/statement/visitor.h"

namespace dreamdb::binder::bound
{

BoundInsertStatement::BoundInsertStatement(
    dreamdb::common::collection_id_t collection_id,
    std::vector<BoundInsertItem> insert_items
)
    : BoundStatement(BoundStatementType::Insert)
    , collection_id_(collection_id)
    , insert_items_(std::move(insert_items))
{
}

void BoundInsertStatement::accept(BoundStatementVisitor & visitor) const
{
    visitor.visit(*this);
}

dreamdb::common::collection_id_t BoundInsertStatement::collection_id() const noexcept
{
    return collection_id_;
}

std::size_t BoundInsertStatement::insert_item_count() const noexcept
{
    return insert_items_.size();
}

const BoundInsertItem & BoundInsertStatement::insert_item_at(std::size_t index) const noexcept
{
    // 索引不能超出范围
    assert(index < insert_items_.size());

    return insert_items_[index];
}

} // namespace dreamdb::binder::bound
