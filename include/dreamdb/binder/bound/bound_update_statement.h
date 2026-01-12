#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "dreamdb/binder/bound/bound_statement.h"
#include "dreamdb/expression/expression.h"

namespace dreamdb
{

struct BoundUpdateItem
{
    std::unique_ptr<Expression> column_reference;   // 列引用
    std::unique_ptr<Expression> value;              // 值
};

class BoundUpdateStatement : public BoundStatement
{
public:
    explicit BoundUpdateStatement();

    ~BoundUpdateStatement() noexcept override = default;

public:
    std::size_t collection_id;                  // 集合 ID
    std::vector<BoundUpdateItem> update_items;  // 更新项
    std::unique_ptr<Expression> where;          // 条件表达式
};

} // namespace dreamdb
