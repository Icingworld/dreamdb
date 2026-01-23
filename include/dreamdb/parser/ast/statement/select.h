#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <variant>

#include "dreamdb/parser/ast/statement/statement.h"
#include "dreamdb/common/type.h"

namespace dreamdb::parser::ast
{

class AstExpression;

/**
 * @brief SELECT 星号条目
 */
struct AstSelectStarItem
{
};

/**
 * @brief SELECT 表达式条目
 */
struct AstSelectExpressionItem
{
    AstSelectExpressionItem(std::unique_ptr<AstExpression> expression, std::optional<std::string> alias = std::nullopt);

    std::unique_ptr<AstExpression> expression;  // 表达式
    std::optional<std::string> alias;           // 可选别名
};

/**
 * @brief SELECT 列表条目
 */
using AstSelectItem = std::variant<
    AstSelectStarItem,          // 星号条目
    AstSelectExpressionItem     // 表达式条目
>;

/**
 * @brief ORDER BY 子句的排序项
 */
struct AstOrderByItem
{
    AstOrderByItem(std::unique_ptr<AstExpression> expression, common::Direction direction = common::Direction::ASC);

    std::unique_ptr<AstExpression> expression;  // 排序表达式
    common::Direction direction;                // 排序方向
};

/**
 * @brief SELECT 语句
 */
class AstSelectStatement final : public AstStatement
{
public:
    AstSelectStatement(
        std::string collection_name,
        std::vector<AstSelectItem> select_items,
        std::unique_ptr<AstExpression> where,
        std::vector<std::unique_ptr<AstExpression>> group_by,
        std::unique_ptr<AstExpression> having,
        std::vector<AstOrderByItem> order_by,
        std::optional<std::uint64_t> limit,
        std::optional<std::uint64_t> offset,
        std::size_t line,
        std::size_t column
    );

    ~AstSelectStatement() noexcept override;

public:
    /**
     * @brief 创建 SELECT 语句
     * @param collection_name 集合名
     * @param select_items SELECT 项列表
     * @param where WHERE 子句
     * @param group_by GROUP BY 子句列表
     * @param having HAVING 子句
     * @param order_by ORDER BY 项列表
     * @param limit LIMIT 子句
     * @param offset OFFSET 子句
     * @param line 行号
     * @param column 列号
     * @return SELECT 语句
     */
    static std::unique_ptr<AstSelectStatement> create(
        std::string collection_name,
        std::vector<AstSelectItem> select_items,
        std::unique_ptr<AstExpression> where,
        std::vector<std::unique_ptr<AstExpression>> group_by,
        std::unique_ptr<AstExpression> having,
        std::vector<AstOrderByItem> order_by,
        std::optional<std::uint64_t> limit,
        std::optional<std::uint64_t> offset,
        std::size_t line,
        std::size_t column
    );

public:
    /**
     * @brief 获取集合名
     * @return 集合名
     */
    const std::string & collection_name() const noexcept;

    /**
     * @brief 获取指定索引的 SELECT 项
     * @param index 索引
     * @return SELECT 项
     */
    const AstSelectItem & select_item_at(std::size_t index) const noexcept;

    /**
     * @brief 获取 SELECT 项数量
     * @return SELECT 项数量
     */
    std::size_t select_item_count() const noexcept;

    /**
     * @brief 是否存在 WHERE 子句
     * @return 是否存在 WHERE 子句
     */
    bool has_where() const noexcept;

    /**
     * @brief 获取 WHERE 子句
     * @return WHERE 子句
     */
    const AstExpression * where() const noexcept;

    /**
     * @brief 获取 WHERE 子句
     * @return WHERE 子句
     */
    const AstExpression & where_ref() const noexcept;

    /**
     * @brief 获取指定索引的 GROUP BY 项
     * @param index 索引
     * @return GROUP BY 项
     */
    const AstExpression & group_by_at(std::size_t index) const noexcept;

    /**
     * @brief 获取 GROUP BY 项数量
     * @return GROUP BY 项数量
     */
    std::size_t group_by_count() const noexcept;

    /**
     * @brief 是否存在 HAVING 子句
     * @return 是否存在 HAVING 子句
     */
    bool has_having() const noexcept;

    /**
     * @brief 获取 HAVING 子句
     * @return HAVING 子句
     */
    const AstExpression * having() const noexcept;

    /**
     * @brief 获取 HAVING 子句
     * @return HAVING 子句
     */
    const AstExpression & having_ref() const noexcept;

    /**
     * @brief 获取指定索引的 ORDER BY 项
     * @param index 索引
     * @return ORDER BY 项
     */
    const AstOrderByItem & order_by_item_at(std::size_t index) const noexcept;

    /**
     * @brief 获取 ORDER BY 项数量
     * @return ORDER BY 项数量
     */
    std::size_t order_by_item_count() const noexcept;

    /**
     * @brief 获取 LIMIT 值
     * @return LIMIT 值
     */
    const std::optional<std::uint64_t> & limit() const noexcept;

    /**
     * @brief 获取 OFFSET 值
     * @return OFFSET 值
     */
    const std::optional<std::uint64_t> & offset() const noexcept;

    /**
     * @brief 接受语句访问者
     * @param visitor 语句访问者
     */
    void accept(AstStatementVisitor & visitor) const override;

private:
    std::string collection_name_;                              // 集合名
    std::vector<AstSelectItem> select_items_;                  // SELECT 项列表
    std::unique_ptr<AstExpression> where_;                     // WHERE 子句
    std::vector<std::unique_ptr<AstExpression>> group_by_;     // GROUP BY 子句列表
    std::unique_ptr<AstExpression> having_;                    // HAVING 子句
    std::vector<AstOrderByItem> order_by_items_;               // ORDER BY 项列表
    std::optional<std::uint64_t> limit_;                       // LIMIT 值
    std::optional<std::uint64_t> offset_;                      // OFFSET 值
};

} // namespace dreamdb::parser::ast
