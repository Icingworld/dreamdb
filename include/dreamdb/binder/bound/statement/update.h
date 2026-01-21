#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "dreamdb/binder/bound/statement/statement.h"
#include "dreamdb/binder/bound/expression/expression.h"
#include "dreamdb/common/ids.h"

namespace dreamdb::binder::bound
{

class BoundStatementVisitor;

/**
 * @brief 绑定后的更新项
 */
struct BoundUpdateItem
{
    std::unique_ptr<BoundExpression> column_reference;  // 列引用
    std::unique_ptr<BoundExpression> value;             // 值表达式
};

/**
 * @brief 绑定后的 UPDATE 语句
 */
class BoundUpdateStatement final : public BoundStatement
{
public:
    explicit BoundUpdateStatement(
        dreamdb::common::collection_id_t collection_id,
        std::vector<BoundUpdateItem> update_items,
        std::unique_ptr<BoundExpression> where
    );

    ~BoundUpdateStatement() noexcept override = default;

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
     * @brief 获取更新项数量
     * @return 更新项数量
     */
    std::size_t update_item_count() const noexcept;

    /**
     * @brief 获取更新项
     * @param index 索引
     * @return 更新项
     */
    const BoundUpdateItem & update_item_at(std::size_t index) const noexcept;

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
    dreamdb::common::collection_id_t collection_id_;  // 集合 ID
    std::vector<BoundUpdateItem> update_items_;       // 更新项
    std::unique_ptr<BoundExpression> where_;          // WHERE 子句
};

} // namespace dreamdb::binder::bound
