#pragma once

#include <cstddef>

#include "dreamdb/binder/bound/bound_statement.h"

namespace dreamdb
{

/**
 * @brief 绑定后的 DESCRIBE 语句
 */
class BoundDescribeStatement : public BoundStatement
{
public:
    explicit BoundDescribeStatement();

    ~BoundDescribeStatement() noexcept override = default;

public:
    std::size_t collection_id;  // 集合 ID
};

} // namespace dreamdb
