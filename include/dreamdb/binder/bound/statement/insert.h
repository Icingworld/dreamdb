#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "dreamdb/binder/bound/expression/expression.h"
#include "dreamdb/binder/bound/statement/statement.h"
#include "dreamdb/common/ids.h"

namespace dreamdb::binder::bound
{

class BoundStatementVisitor;

/**
 * @brief 绑定后的插入项
 */
struct BoundInsertItem
{
    std::unique_ptr<BoundExpression> column_reference;  // 列引用
    std::unique_ptr<BoundExpression> value;             // 值表达式
};

/**
 * @brief 绑定后的 INSERT 语句
 */
class BoundInsertStatement final : public BoundStatement
{
public:
    explicit BoundInsertStatement(
        dreamdb::common::collection_id_t collection_id,
        std::vector<BoundInsertItem> insert_items
    );

    ~BoundInsertStatement() noexcept override = default;

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
     * @brief 获取插入项数量
     * @return 插入项数量
     */
    std::size_t insert_item_count() const noexcept;

    /**
     * @brief 获取插入项
     * @param index 索引
     * @return 插入项
     */
    const BoundInsertItem & insert_item_at(std::size_t index) const noexcept;

private:
    dreamdb::common::collection_id_t collection_id_;   // 集合 ID
    std::vector<BoundInsertItem> insert_items_;        // 插入项
};

} // namespace dreamdb::binder::bound
