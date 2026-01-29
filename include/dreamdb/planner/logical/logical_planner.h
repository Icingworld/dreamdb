#pragma once

#include <memory>
#include <vector>

#include "dreamdb/binder/bound/expression/visitor.h"
#include "dreamdb/common/ids.h"

namespace dreamdb::binder::bound
{

class BoundStatement;
class BoundSelectStatement;
class BoundDeleteStatement;
class BoundUpdateStatement;
class BoundExpression;

} // namespace dreamdb::binder::bound

namespace dreamdb::planner::logical
{

class LogicalOperator;

/**
 * @brief 绑定列收集器
 */
class BoundColumnCollector : public dreamdb::binder::bound::BoundExpressionVisitor
{
public:
    explicit BoundColumnCollector() noexcept = default;

    ~BoundColumnCollector() noexcept override = default;

public:
    void visit(const dreamdb::binder::bound::BoundColumnReferenceExpression & column_reference_expression) override;

    void visit(const dreamdb::binder::bound::BoundConstantExpression & constant_expression) override;

    void visit(const dreamdb::binder::bound::BoundFunctionCallExpression & function_call_expression) override;

    void visit(const dreamdb::binder::bound::BoundInExpression & in_expression) override;

    void visit(const dreamdb::binder::bound::BoundBetweenExpression & between_expression) override;

    void visit(const dreamdb::binder::bound::BoundLikeExpression & like_expression) override;

    void visit(const dreamdb::binder::bound::BoundUnaryExpression & unary_expression) override;

    void visit(const dreamdb::binder::bound::BoundBinaryExpression & binary_expression) override;

public:
    std::vector<dreamdb::common::column_oid_t> column_ids;
};

/**
 * @brief 逻辑计划器
 */
class LogicalPlanner
{
public:
    explicit LogicalPlanner() noexcept = default;

    ~LogicalPlanner() noexcept = default;

public:
    /**
     * @brief 计划节点
     * @param bound_statement 绑定后的语句
     * @return 逻辑计划节点
     */
    std::unique_ptr<LogicalOperator> plan(
        const dreamdb::binder::bound::BoundStatement & bound_statement
    ) const;

private:
    /**
     * @brief 计划 SELECT 语句
     * @param bound_select_statement 绑定后的 SELECT 语句
     * @return 逻辑计划节点
     */
    std::unique_ptr<LogicalOperator> plan_select(
        const dreamdb::binder::bound::BoundSelectStatement & select_statement
    ) const;

    /**
     * @brief 计划 DELETE 语句
     * @param bound_delete_statement 绑定后的 DELETE 语句
     * @return 逻辑计划节点
     */
    std::unique_ptr<LogicalOperator> plan_delete(
        const dreamdb::binder::bound::BoundDeleteStatement & delete_statement
    ) const;

    /**
     * @brief 计划 UPDATE 语句
     * @param bound_update_statement 绑定后的 UPDATE 语句
     * @return 逻辑计划节点
     */
    std::unique_ptr<LogicalOperator> plan_update(
        const dreamdb::binder::bound::BoundUpdateStatement & update_statement
    ) const;

    /**
     * @brief 收集列 ID
     * @param bound_expression 绑定后的表达式
     * @return 列 ID 列表
     */
    static std::vector<dreamdb::common::column_oid_t> collect_column_ids(
        const dreamdb::binder::bound::BoundExpression & bound_expression
    );
};

} // namespace dreamdb::planner::logical
