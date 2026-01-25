#pragma once

#include "dreamdb/planner/logical/logical_operator.h"
#include "dreamdb/common/ids.h"

namespace dreamdb::planner::logical
{

/**
 * @brief 逻辑删除算子
 */
class LogicalDelete final : public LogicalOperator
{
public:
    LogicalDelete(dreamdb::common::collection_id_t collection_id);

    ~LogicalDelete() noexcept override = default;

public:
    /**
     * @brief 获取集合 ID
     * @return 集合 ID
     */
    dreamdb::common::collection_id_t collection_id() const noexcept;

private:
    dreamdb::common::collection_id_t collection_id_;    // 集合 ID
};

} // namespace dreamdb::planner::logical
