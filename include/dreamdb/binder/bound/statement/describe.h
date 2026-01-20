#pragma once

#include "dreamdb/binder/bound/statement/statement.h"
#include "dreamdb/common/ids.h"

namespace dreamdb::binder::bound
{

/**
 * @brief 绑定后的 DESCRIBE 语句
 */
class BoundDescribeStatement final : public BoundStatement
{
public:
    explicit BoundDescribeStatement(dreamdb::common::collection_id_t collection_id) noexcept;

    ~BoundDescribeStatement() noexcept override = default;

public:
    /**
     * @brief 获取集合 ID
     * @return 集合 ID
     */
    dreamdb::common::collection_id_t collection_id() const noexcept;

private:
    dreamdb::common::collection_id_t collection_id_;  // 集合 ID
};

} // namespace dreamdb::binder::bound
