#include "dreamdb/binder/bound/bound_select_statement.h"

namespace dreamdb
{

BoundSelectStatement::BoundSelectStatement()
    : BoundStatement(BoundStatementType::BINDER_BOUND_SELECT_STATEMENT)
    , select_items()
    , collection_id(0)
    , where(nullptr)
    , group_by()
    , having(nullptr)
    , order_by()
    , limit(std::nullopt)
    , offset(std::nullopt)
{
}

} // namespace dreamdb
