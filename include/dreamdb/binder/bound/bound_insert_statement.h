#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "dreamdb/expression/expression.h"
#include "dreamdb/binder/bound/bound_statement.h"

namespace dreamdb
{

struct BoundInsertItem
{
    std::unique_ptr<Expression> column_reference;       // 列引用
    std::unique_ptr<Expression> value;                  // 值
};

class BoundInsertStatement : public BoundStatement
{
public:
    explicit BoundInsertStatement();

    ~BoundInsertStatement() noexcept override = default;

public:
    std::size_t collection_id;                     // 集合 ID
    std::vector<BoundInsertItem> insert_items;     // 插入项
};

} // namespace dreamdb
