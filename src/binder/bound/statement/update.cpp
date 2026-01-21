#include "dreamdb/binder/bound/statement/update.h"

#include <cassert>

#include "dreamdb/binder/bound/statement/visitor.h"

namespace dreamdb::binder::bound
{

BoundUpdateStatement::BoundUpdateStatement(
    dreamdb::common::collection_id_t collection_id,
    std::vector<BoundUpdateItem> update_items,
    std::unique_ptr<BoundExpression> where
)
    : BoundStatement(BoundStatementType::Update)
    , collection_id_(collection_id)
    , update_items_(std::move(update_items))
    , where_(std::move(where))
{
    // 更新项不能为空
    assert(!update_items.empty());

    // 每一项更新项的列引用和值表达式不能为空
    for (const auto & update_item : update_items_) {
        assert(update_item.column_reference != nullptr);
        assert(update_item.value != nullptr);
    }
}

void BoundUpdateStatement::accept(BoundStatementVisitor & visitor) const
{
    visitor.visit(*this);
}

dreamdb::common::collection_id_t BoundUpdateStatement::collection_id() const noexcept
{
    return collection_id_;
}

std::size_t BoundUpdateStatement::update_item_count() const noexcept
{
    return update_items_.size();
}

const BoundUpdateItem & BoundUpdateStatement::update_item_at(std::size_t index) const noexcept
{
    // 索引不能超出范围
    assert(index < update_items_.size());

    return update_items_[index];
}

bool BoundUpdateStatement::has_where() const noexcept
{
    return where_ != nullptr;
}

const BoundExpression * BoundUpdateStatement::where() const noexcept
{
    return where_.get();
}

const BoundExpression & BoundUpdateStatement::where_ref() const noexcept
{
    // WHERE 子句必须存在
    assert(where_ != nullptr);

    return *where_;
}

} // namespace dreamdb::binder::bound
