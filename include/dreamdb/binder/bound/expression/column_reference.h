#pragma once

#include "dreamdb/binder/bound/expression/expression.h"
#include "dreamdb/common/ids.h"
#include "dreamdb/common/logical_type.h"

namespace dreamdb::binder::bound
{

class BoundExpressionVisitor;

/**
 * @brief 字段引用表达式
 */
class BoundColumnReferenceExpression final : public BoundExpression
{
public:
    explicit BoundColumnReferenceExpression(dreamdb::common::column_id_t column_id, dreamdb::common::LogicalType logical_type) noexcept;

    ~BoundColumnReferenceExpression() noexcept override = default;

public:
    /**
     * @brief 获取列 ID
     * @return 列 ID
     */
    dreamdb::common::column_id_t column_id() const noexcept;

    /**
     * @brief 接受表达式访问者
     * @param visitor 表达式访问者
     */
    void accept(BoundExpressionVisitor & visitor) const override;

    /**
     * @brief 克隆表达式
     * @return 克隆后的表达式
     */
    std::unique_ptr<BoundExpression> clone() const override;

private:
    dreamdb::common::column_id_t column_id_;  // 列 ID
};

} // namespace dreamdb::binder::bound
