#include "dreamdb/binder/bound/statement/select.h"

#include <cassert>

#include "dreamdb/binder/bound/statement/visitor.h"

namespace dreamdb::binder::bound
{

BoundSelectStatement::BoundSelectStatement(
    dreamdb::common::collection_id_t collection_id,
    std::vector<BoundSelectItem> select_items,
    std::unique_ptr<BoundExpression> where,
    std::vector<std::unique_ptr<BoundExpression>> group_by,
    std::unique_ptr<BoundExpression> having,
    std::vector<BoundOrderByItem> order_by,
    std::optional<std::size_t> limit,
    std::optional<std::size_t> offset
)
    : BoundStatement(BoundStatementType::Select)
    , collection_id_(collection_id)
    , select_items_(std::move(select_items))
    , where_(std::move(where))
    , group_by_(std::move(group_by))
    , having_(std::move(having))
    , order_by_(std::move(order_by))
    , limit_(limit)
    , offset_(offset)
{
}

void BoundSelectStatement::accept(BoundStatementVisitor & visitor) const
{
    visitor.visit(*this);
}

dreamdb::common::collection_id_t BoundSelectStatement::collection_id() const noexcept
{
    return collection_id_;
}

std::size_t BoundSelectStatement::select_item_count() const noexcept
{
    return select_items_.size();
}

const BoundSelectItem & BoundSelectStatement::select_item_at(std::size_t index) const noexcept
{
    assert(index < select_items_.size());
    return select_items_[index];
}

bool BoundSelectStatement::has_where() const noexcept
{
    return where_ != nullptr;
}

const BoundExpression * BoundSelectStatement::where() const noexcept
{
    return where_.get();
}

const BoundExpression & BoundSelectStatement::where_ref() const noexcept
{
    assert(where_ != nullptr);
    return *where_;
}

std::size_t BoundSelectStatement::group_by_count() const noexcept
{
    return group_by_.size();
}

const BoundExpression * BoundSelectStatement::group_by_at(std::size_t index) const noexcept
{
    assert(index < group_by_.size());
    return group_by_[index].get();
}

bool BoundSelectStatement::has_having() const noexcept
{
    return having_ != nullptr;
}

const BoundExpression * BoundSelectStatement::having() const noexcept
{
    return having_.get();
}

const BoundExpression & BoundSelectStatement::having_ref() const noexcept
{
    assert(having_ != nullptr);
    return *having_;
}

std::size_t BoundSelectStatement::order_by_count() const noexcept
{
    return order_by_.size();
}

const BoundOrderByItem & BoundSelectStatement::order_by_at(std::size_t index) const noexcept
{
    assert(index < order_by_.size());
    return order_by_[index];
}

bool BoundSelectStatement::has_limit() const noexcept
{
    return limit_.has_value();
}

std::size_t BoundSelectStatement::limit() const noexcept
{
    assert(limit_.has_value());
    return limit_.value();
}

bool BoundSelectStatement::has_offset() const noexcept
{
    return offset_.has_value();
}

std::size_t BoundSelectStatement::offset() const noexcept
{
    assert(offset_.has_value());
    return offset_.value();
}

} // namespace dreamdb::binder::bound
