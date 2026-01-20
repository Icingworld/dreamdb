#pragma once

#include <cstddef>

#include "dreamdb/binder/bound/expression/expression.h"
#include "dreamdb/common/ids.h"
#include "dreamdb/common/logical_type.h"

namespace dreamdb::binder::bound
{

/**
 * @brief 字段引用表达式
 */
class ColumnReferenceExpression final : public BoundExpression
{
public:
    explicit ColumnReferenceExpression(dreamdb::common::column_id_t column_id, dreamdb::common::LogicalType logical_type) noexcept;

    ~ColumnReferenceExpression() noexcept override = default;

public:
    /**
     * @brief 获取列 ID
     * @return 列 ID
     */
    dreamdb::common::column_id_t column_id() const noexcept;

private:
    dreamdb::common::column_id_t column_id_;  // 列 ID
};

} // namespace dreamdb::binder::bound
