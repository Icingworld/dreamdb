#pragma once

#include <cstddef>

#include "dreamdb/binder/bound/bound_statement.h"

namespace dreamdb
{

/**
 * @brief 绑定后的 USE 语句
 */
class BoundUseStatement : public BoundStatement
{
public:
    explicit BoundUseStatement();

    ~BoundUseStatement() noexcept override = default;

public:
    std::size_t database_id;  // 数据库 ID
};

} // namespace dreamdb
