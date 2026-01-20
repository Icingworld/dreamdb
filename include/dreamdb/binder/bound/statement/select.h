#pragma once

#include <memory>
#include <string>
#include <vector>
#include <optional>

#include "dreamdb/binder/bound/statement/statement.h"
#include "dreamdb/binder/bound/expression/expression.h"
#include "dreamdb/common/ids.h"
#include "dreamdb/common/type.h"

namespace dreamdb::binder::bound
{

/**
 * @brief 绑定后的选择项
 */
struct BoundSelectItem
{
    std::unique_ptr<BoundExpression> expr;  // 表达式
    std::string alias;                      // 输出别名，允许为空
};

/**
 * @brief 绑定后的排序项
 */
struct BoundOrderByItem
{
    std::unique_ptr<BoundExpression> expr;  // 排序表达式
    dreamdb::Direction order;                // 排序方向
};

/**
 * @brief 绑定后的 SELECT 语句
 */
class BoundSelectStatement final : public BoundStatement
{
public:
    explicit BoundSelectStatement(
        dreamdb::common::collection_id_t collection_id,
        std::vector<BoundSelectItem> select_items,
        std::unique_ptr<BoundExpression> where,
        std::vector<std::unique_ptr<BoundExpression>> group_by,
        std::unique_ptr<BoundExpression> having,
        std::vector<BoundOrderByItem> order_by,
        std::optional<std::size_t> limit,
        std::optional<std::size_t> offset
    );

    ~BoundSelectStatement() noexcept override = default;

public:
    /**
     * @brief 获取集合 ID
     * @return 集合 ID
     */
    dreamdb::common::collection_id_t collection_id() const noexcept;

    /**
     * @brief 获取选择项数量
     * @return 选择项数量
     */
    std::size_t select_item_count() const noexcept;

    /**
     * @brief 获取选择项
     * @param index 索引
     * @return 选择项
     */
    const BoundSelectItem & select_item_at(std::size_t index) const noexcept;

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

    /**
     * @brief 获取 GROUP BY 子句数量
     * @return GROUP BY 子句数量
     */
    std::size_t group_by_count() const noexcept;

    /**
     * @brief 获取 GROUP BY 子句
     * @param index 索引
     * @return GROUP BY 表达式
     */
    const BoundExpression * group_by_at(std::size_t index) const noexcept;

    /**
     * @brief 是否存在 HAVING 子句
     * @return 是否存在 HAVING 子句
     */
    bool has_having() const noexcept;

    /**
     * @brief 获取 HAVING 子句
     * @return HAVING 子句
     */
    const BoundExpression * having() const noexcept;

    /**
     * @brief 获取 HAVING 子句引用
     * @return HAVING 子句引用
     */
    const BoundExpression & having_ref() const noexcept;

    /**
     * @brief 获取 ORDER BY 项数量
     * @return ORDER BY 项数量
     */
    std::size_t order_by_count() const noexcept;

    /**
     * @brief 获取 ORDER BY 项
     * @param index 索引
     * @return ORDER BY 项
     */
    const BoundOrderByItem & order_by_at(std::size_t index) const noexcept;

    /**
     * @brief 是否存在 LIMIT 子句
     * @return 是否存在 LIMIT 子句
     */
    bool has_limit() const noexcept;

    /**
     * @brief 获取 LIMIT 值
     * @return LIMIT 值
     */
    std::size_t limit() const noexcept;

    /**
     * @brief 是否存在 OFFSET 子句
     * @return 是否存在 OFFSET 子句
     */
    bool has_offset() const noexcept;

    /**
     * @brief 获取 OFFSET 值
     * @return OFFSET 值
     */
    std::size_t offset() const noexcept;

private:
    dreamdb::common::collection_id_t collection_id_;              // 集合 ID
    std::vector<BoundSelectItem> select_items_;                   // 选择项
    std::unique_ptr<BoundExpression> where_;                      // 条件表达式
    std::vector<std::unique_ptr<BoundExpression>> group_by_;      // 分组表达式
    std::unique_ptr<BoundExpression> having_;                     // 过滤表达式
    std::vector<BoundOrderByItem> order_by_;                      // 排序表达式
    std::optional<std::size_t> limit_;                            // 限制
    std::optional<std::size_t> offset_;                           // 偏移
};

} // namespace dreamdb::binder::bound
