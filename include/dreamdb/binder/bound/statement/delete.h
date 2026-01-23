#pragma once

#include <memory>

#include "dreamdb/binder/bound/statement/statement.h"
#include "dreamdb/binder/bound/expression/expression.h"
#include "dreamdb/common/ids.h"

namespace dreamdb::binder::bound
{

class BoundStatementVisitor;

/**
 * @brief 绑定后的 DELETE 语句
 */
class BoundDeleteStatement final : public BoundStatement
{
public:
    explicit BoundDeleteStatement(dreamdb::common::collection_id_t collection_id, std::unique_ptr<BoundExpression> where);

    ~BoundDeleteStatement() noexcept override = default;

public:
    /**
     * @brief 接受语句访问者
     * @param visitor 语句访问者
     */
    void accept(BoundStatementVisitor & visitor) const override;

public:
    /**
     * @brief 获取集合 ID
     * @return 集合 ID
     */
    dreamdb::common::collection_id_t collection_id() const noexcept;

    /**
     * @brief 是否存在 WHERE 子句
     * @return 是否存在 WHERE 子句
     */
    bool has_where() const noexcept;

    /**
     * @brief 获取 WHERE 子句
     * @return WHERE 子句
     */
    const BoundExpression * where() const noexcept;

    /**
     * @brief 获取 WHERE 子句引用
     * @return WHERE 子句引用
     */
    const BoundExpression & where_ref() const noexcept;

private:
    dreamdb::common::collection_id_t collection_id_;   // 集合 ID
    std::unique_ptr<BoundExpression> where_;           // WHERE 子句
};

} // namespace dreamdb::binder::bound
