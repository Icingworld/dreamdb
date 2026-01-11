#pragma once

#include <cstddef>
#include <memory>

#include "dreamdb/binder/bound/bound_statement.h"
#include "dreamdb/expression/expression.h"

namespace dreamdb
{

class BoundDeleteStatement : public BoundStatement
{
public:
    explicit BoundDeleteStatement();

    ~BoundDeleteStatement() noexcept override = default;

public:
    std::size_t collection_id;             // 集合 ID
    std::unique_ptr<Expression> where;     // 条件表达式
};

} // namespace dreamdb
