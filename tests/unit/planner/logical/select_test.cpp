#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <dreamdb/binder/binder.h>
#include <dreamdb/binder/bound/statement/select.h>
#include <dreamdb/parser/ast/statement/use.h>
#include <dreamdb/parser/ast/statement/select.h>
#include <dreamdb/parser/ast/expression/literal.h>
#include <dreamdb/parser/ast/expression/column_reference.h>
#include <dreamdb/parser/ast/expression/binary.h>
#include <dreamdb/parser/ast/expression/function_call.h>
#include <dreamdb/planner/logical/logical_planner.h>
#include <dreamdb/planner/logical/operator/logical_scan.h>
#include <dreamdb/planner/logical/operator/logical_filter.h>
#include <dreamdb/planner/logical/operator/logical_project.h>
#include <dreamdb/planner/logical/operator/logical_aggregate.h>
#include <dreamdb/planner/logical/operator/logical_sort.h>
#include <dreamdb/planner/logical/operator/logical_limit_offset.h>
#include <dreamdb/common/type.h>

#include "../../binder/catalog_mock.h"

using ::testing::Return;

class SelectLogicalPlannerTest : public ::testing::Test
{
public:
    SelectLogicalPlannerTest()
        : catalog_mock()
        , binder(catalog_mock)
        , planner()
    {
        // 默认行为：解析数据库与集合
        ON_CALL(catalog_mock, resolve_database("test_db"))
            .WillByDefault(Return(std::make_optional<dreamdb::common::database_id_t>(12345)));

        ON_CALL(catalog_mock, resolve_collection(
            dreamdb::common::database_id_t{12345}, "test_collection"
        ))
            .WillByDefault(Return(std::make_optional<dreamdb::common::collection_id_t>(99)));

        // 设置列信息（用于 get_columns）
        std::vector<dreamdb::catalog::column_info_t> test_collection_columns;
        
        dreamdb::catalog::column_info_t id_column_info{
            dreamdb::common::column_id_t{0},
            0,  // index
            "id",
            dreamdb::common::LogicalType{dreamdb::common::LogicalTypeId::Integer},
            false,  // is_nullable (NOT NULL)
            true    // is_insertable
        };
        test_collection_columns.push_back(id_column_info);

        dreamdb::catalog::column_info_t name_column_info{
            dreamdb::common::column_id_t{1},
            1,  // index
            "name",
            dreamdb::common::LogicalType{dreamdb::common::LogicalTypeId::String},
            true,  // is_nullable
            true   // is_insertable
        };
        test_collection_columns.push_back(name_column_info);

        dreamdb::catalog::column_info_t age_column_info{
            dreamdb::common::column_id_t{2},
            2,  // index
            "age",
            dreamdb::common::LogicalType{dreamdb::common::LogicalTypeId::Integer},
            true,  // is_nullable
            true   // is_insertable
        };
        test_collection_columns.push_back(age_column_info);

        ON_CALL(catalog_mock, get_columns(
            dreamdb::common::collection_id_t{99}
        ))
            .WillByDefault(Return(test_collection_columns));

        // 设置列解析（用于 resolve_column）
        ON_CALL(catalog_mock, resolve_column(
            dreamdb::common::collection_id_t{99}, "id"
        ))
            .WillByDefault(Return(std::make_optional<dreamdb::catalog::column_info_t>(id_column_info)));

        ON_CALL(catalog_mock, resolve_column(
            dreamdb::common::collection_id_t{99}, "name"
        ))
            .WillByDefault(Return(std::make_optional<dreamdb::catalog::column_info_t>(name_column_info)));

        ON_CALL(catalog_mock, resolve_column(
            dreamdb::common::collection_id_t{99}, "age"
        ))
            .WillByDefault(Return(std::make_optional<dreamdb::catalog::column_info_t>(age_column_info)));
    }

public:
    CatalogMock catalog_mock;
    dreamdb::binder::Binder binder;
    dreamdb::planner::logical::LogicalPlanner planner;
};

/**
 * @brief 测试计划简单 SELECT 语句（无 WHERE 子句）
 * @details 应该生成 Scan -> Project 的计划树
 */
TEST_F(SelectLogicalPlannerTest, PlanSimpleSelect)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 SELECT 语句：SELECT id, name FROM test_collection
    std::vector<dreamdb::parser::ast::AstSelectItem> select_items;
    auto id_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(std::move(id_column), std::nullopt));
    auto name_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("name", 1, 1);
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(std::move(name_column), std::nullopt));

    auto ast_select_statement = dreamdb::parser::ast::AstSelectStatement::create(
        "test_collection",
        std::move(select_items),
        nullptr,  // 无 WHERE 子句
        {},       // 无 GROUP BY 子句
        nullptr,  // 无 HAVING 子句
        {},       // 无 ORDER BY 子句
        std::nullopt,  // 无 LIMIT
        std::nullopt,  // 无 OFFSET
        1, 1
    );

    // 绑定语句
    auto bound_select_statement = binder.bind(*ast_select_statement);
    ASSERT_NE(bound_select_statement, nullptr);

    // 计划语句
    auto logical_plan = planner.plan(*bound_select_statement);
    ASSERT_NE(logical_plan, nullptr);

    // 验证根节点是 Project 算子
    EXPECT_EQ(logical_plan->operator_type(), dreamdb::planner::logical::LogicalOperatorType::Project);
    EXPECT_EQ(logical_plan->child_count(), 1);

    // 验证 Project 算子的属性
    const auto * project_operator = dynamic_cast<const dreamdb::planner::logical::LogicalProject *>(logical_plan.get());
    ASSERT_NE(project_operator, nullptr);
    EXPECT_EQ(project_operator->project_item_count(), 2);

    // 验证子节点是 Scan 算子
    const auto & scan_operator = logical_plan->child_at(0);
    EXPECT_EQ(scan_operator.operator_type(), dreamdb::planner::logical::LogicalOperatorType::Scan);
    EXPECT_EQ(scan_operator.child_count(), 0);

    const auto * scan = dynamic_cast<const dreamdb::planner::logical::LogicalScan *>(&scan_operator);
    ASSERT_NE(scan, nullptr);
    EXPECT_EQ(scan->collection_id(), 99);
    EXPECT_EQ(scan->column_count(), 2);  // id 和 name 两列
    EXPECT_EQ(scan->column_at(0), dreamdb::common::column_id_t{0});  // id
    EXPECT_EQ(scan->column_at(1), dreamdb::common::column_id_t{1});  // name
}

/**
 * @brief 测试计划 SELECT 语句（带 WHERE 子句）
 * @details 应该生成 Scan -> Filter -> Project 的计划树
 */
TEST_F(SelectLogicalPlannerTest, PlanSelectWithWhere)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 WHERE 子句：id = 1
    auto id_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    auto literal_one = dreamdb::parser::ast::AstLiteralExpression::create_integer(1, 1, 1);
    auto where_expr = dreamdb::parser::ast::AstBinaryExpression::create(
        dreamdb::parser::ast::AstBinaryOperatorType::Equal,
        std::move(id_column),
        std::move(literal_one),
        1, 1
    );

    // 创建 SELECT 语句：SELECT name FROM test_collection WHERE id = 1
    std::vector<dreamdb::parser::ast::AstSelectItem> select_items;
    auto name_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("name", 1, 1);
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(std::move(name_column), std::nullopt));

    auto ast_select_statement = dreamdb::parser::ast::AstSelectStatement::create(
        "test_collection",
        std::move(select_items),
        std::move(where_expr),
        {},       // 无 GROUP BY 子句
        nullptr,  // 无 HAVING 子句
        {},       // 无 ORDER BY 子句
        std::nullopt,  // 无 LIMIT
        std::nullopt,  // 无 OFFSET
        1, 1
    );

    // 绑定语句
    auto bound_select_statement = binder.bind(*ast_select_statement);
    ASSERT_NE(bound_select_statement, nullptr);

    // 计划语句
    auto logical_plan = planner.plan(*bound_select_statement);
    ASSERT_NE(logical_plan, nullptr);

    // 验证根节点是 Project 算子
    EXPECT_EQ(logical_plan->operator_type(), dreamdb::planner::logical::LogicalOperatorType::Project);
    EXPECT_EQ(logical_plan->child_count(), 1);

    // 验证子节点是 Filter 算子
    const auto & filter_operator = logical_plan->child_at(0);
    EXPECT_EQ(filter_operator.operator_type(), dreamdb::planner::logical::LogicalOperatorType::Filter);
    EXPECT_EQ(filter_operator.child_count(), 1);

    const auto * filter = dynamic_cast<const dreamdb::planner::logical::LogicalFilter *>(&filter_operator);
    ASSERT_NE(filter, nullptr);
    EXPECT_NE(&filter->predicate(), nullptr);

    // 验证 Filter 的子节点是 Scan 算子
    const auto & scan_operator = filter_operator.child_at(0);
    EXPECT_EQ(scan_operator.operator_type(), dreamdb::planner::logical::LogicalOperatorType::Scan);
    EXPECT_EQ(scan_operator.child_count(), 0);

    const auto * scan = dynamic_cast<const dreamdb::planner::logical::LogicalScan *>(&scan_operator);
    ASSERT_NE(scan, nullptr);
    EXPECT_EQ(scan->collection_id(), 99);
    // WHERE 子句使用了 id 列，SELECT 需要 name 列
    EXPECT_EQ(scan->column_count(), 2);
    EXPECT_EQ(scan->column_at(0), dreamdb::common::column_id_t{0});  // id
    EXPECT_EQ(scan->column_at(1), dreamdb::common::column_id_t{1});  // name
}

/**
 * @brief 测试计划 SELECT 语句（带 GROUP BY 子句）
 * @details 应该生成 Scan -> Aggregate 的计划树
 */
TEST_F(SelectLogicalPlannerTest, PlanSelectWithGroupBy)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 SELECT 语句：SELECT age, COUNT(*) FROM test_collection GROUP BY age
    std::vector<dreamdb::parser::ast::AstSelectItem> select_items;
    auto age_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("age", 1, 1);
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(std::move(age_column), std::nullopt));
    
    // COUNT(*) 函数调用（无参数）
    auto count_func = dreamdb::parser::ast::AstFunctionCallExpression::create(
        "COUNT",
        std::vector<std::unique_ptr<dreamdb::parser::ast::AstExpression>>{},
        1, 1
    );
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(std::move(count_func), std::nullopt));

    // GROUP BY age
    std::vector<std::unique_ptr<dreamdb::parser::ast::AstExpression>> group_by;
    auto group_by_age = dreamdb::parser::ast::AstColumnReferenceExpression::create("age", 1, 1);
    group_by.push_back(std::move(group_by_age));

    auto ast_select_statement = dreamdb::parser::ast::AstSelectStatement::create(
        "test_collection",
        std::move(select_items),
        nullptr,  // 无 WHERE 子句
        std::move(group_by),
        nullptr,  // 无 HAVING 子句
        {},       // 无 ORDER BY 子句
        std::nullopt,  // 无 LIMIT
        std::nullopt,  // 无 OFFSET
        1, 1
    );

    // 绑定语句
    auto bound_select_statement = binder.bind(*ast_select_statement);
    ASSERT_NE(bound_select_statement, nullptr);

    // 计划语句
    auto logical_plan = planner.plan(*bound_select_statement);
    ASSERT_NE(logical_plan, nullptr);

    // 验证根节点是 Aggregate 算子
    EXPECT_EQ(logical_plan->operator_type(), dreamdb::planner::logical::LogicalOperatorType::Aggregate);
    EXPECT_EQ(logical_plan->child_count(), 1);

    // 验证 Aggregate 算子的属性
    const auto * aggregate_operator = dynamic_cast<const dreamdb::planner::logical::LogicalAggregate *>(logical_plan.get());
    ASSERT_NE(aggregate_operator, nullptr);
    EXPECT_EQ(aggregate_operator->group_by_count(), 1);
    EXPECT_EQ(aggregate_operator->aggregate_items_count(), 2);

    // 验证子节点是 Scan 算子
    const auto & scan_operator = logical_plan->child_at(0);
    EXPECT_EQ(scan_operator.operator_type(), dreamdb::planner::logical::LogicalOperatorType::Scan);
    EXPECT_EQ(scan_operator.child_count(), 0);

    const auto * scan = dynamic_cast<const dreamdb::planner::logical::LogicalScan *>(&scan_operator);
    ASSERT_NE(scan, nullptr);
    EXPECT_EQ(scan->collection_id(), 99);
    EXPECT_EQ(scan->column_count(), 1);  // age 列
    EXPECT_EQ(scan->column_at(0), dreamdb::common::column_id_t{2});  // age
}

/**
 * @brief 测试计划 SELECT 语句（带聚合函数，无 GROUP BY）
 * @details 应该生成 Scan -> Aggregate 的计划树
 */
TEST_F(SelectLogicalPlannerTest, PlanSelectWithAggregateFunction)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 SELECT 语句：SELECT COUNT(*) FROM test_collection
    std::vector<dreamdb::parser::ast::AstSelectItem> select_items;
    // COUNT(*) 函数调用（无参数）
    auto count_func = dreamdb::parser::ast::AstFunctionCallExpression::create(
        "COUNT",
        std::vector<std::unique_ptr<dreamdb::parser::ast::AstExpression>>{},
        1, 1
    );
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(std::move(count_func), std::nullopt));

    auto ast_select_statement = dreamdb::parser::ast::AstSelectStatement::create(
        "test_collection",
        std::move(select_items),
        nullptr,  // 无 WHERE 子句
        {},       // 无 GROUP BY 子句
        nullptr,  // 无 HAVING 子句
        {},       // 无 ORDER BY 子句
        std::nullopt,  // 无 LIMIT
        std::nullopt,  // 无 OFFSET
        1, 1
    );

    // 绑定语句
    auto bound_select_statement = binder.bind(*ast_select_statement);
    ASSERT_NE(bound_select_statement, nullptr);

    // 计划语句
    auto logical_plan = planner.plan(*bound_select_statement);
    ASSERT_NE(logical_plan, nullptr);

    // 验证根节点是 Aggregate 算子
    EXPECT_EQ(logical_plan->operator_type(), dreamdb::planner::logical::LogicalOperatorType::Aggregate);
    EXPECT_EQ(logical_plan->child_count(), 1);

    // 验证 Aggregate 算子的属性
    const auto * aggregate_operator = dynamic_cast<const dreamdb::planner::logical::LogicalAggregate *>(logical_plan.get());
    ASSERT_NE(aggregate_operator, nullptr);
    EXPECT_EQ(aggregate_operator->group_by_count(), 0);  // 无 GROUP BY
    EXPECT_EQ(aggregate_operator->aggregate_items_count(), 1);

    // 验证子节点是 Scan 算子
    const auto & scan_operator = logical_plan->child_at(0);
    EXPECT_EQ(scan_operator.operator_type(), dreamdb::planner::logical::LogicalOperatorType::Scan);
    EXPECT_EQ(scan_operator.child_count(), 0);

    const auto * scan = dynamic_cast<const dreamdb::planner::logical::LogicalScan *>(&scan_operator);
    ASSERT_NE(scan, nullptr);
    EXPECT_EQ(scan->collection_id(), 99);
    EXPECT_EQ(scan->column_count(), 0);  // COUNT(*) 不需要扫描任何列
}

/**
 * @brief 测试计划 SELECT 语句（带 HAVING 子句）
 * @details 应该生成 Scan -> Aggregate -> Filter 的计划树
 */
TEST_F(SelectLogicalPlannerTest, PlanSelectWithHaving)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 SELECT 语句：SELECT age, COUNT(*) FROM test_collection GROUP BY age HAVING COUNT(*) > 1
    std::vector<dreamdb::parser::ast::AstSelectItem> select_items;
    auto age_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("age", 1, 1);
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(std::move(age_column), std::nullopt));
    
    // COUNT(*) 函数调用（无参数）
    auto count_func = dreamdb::parser::ast::AstFunctionCallExpression::create(
        "COUNT",
        std::vector<std::unique_ptr<dreamdb::parser::ast::AstExpression>>{},
        1, 1
    );
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(std::move(count_func), std::nullopt));

    // GROUP BY age
    std::vector<std::unique_ptr<dreamdb::parser::ast::AstExpression>> group_by;
    auto group_by_age = dreamdb::parser::ast::AstColumnReferenceExpression::create("age", 1, 1);
    group_by.push_back(std::move(group_by_age));

    // HAVING COUNT(*) > 1
    auto having_count_func = dreamdb::parser::ast::AstFunctionCallExpression::create(
        "COUNT",
        std::vector<std::unique_ptr<dreamdb::parser::ast::AstExpression>>{},
        1, 1
    );
    auto literal_one = dreamdb::parser::ast::AstLiteralExpression::create_integer(1, 1, 1);
    auto having_expr = dreamdb::parser::ast::AstBinaryExpression::create(
        dreamdb::parser::ast::AstBinaryOperatorType::GreaterThan,
        std::move(having_count_func),
        std::move(literal_one),
        1, 1
    );

    auto ast_select_statement = dreamdb::parser::ast::AstSelectStatement::create(
        "test_collection",
        std::move(select_items),
        nullptr,  // 无 WHERE 子句
        std::move(group_by),
        std::move(having_expr),
        {},       // 无 ORDER BY 子句
        std::nullopt,  // 无 LIMIT
        std::nullopt,  // 无 OFFSET
        1, 1
    );

    // 绑定语句
    auto bound_select_statement = binder.bind(*ast_select_statement);
    ASSERT_NE(bound_select_statement, nullptr);

    // 计划语句
    auto logical_plan = planner.plan(*bound_select_statement);
    ASSERT_NE(logical_plan, nullptr);

    // 验证根节点是 Filter 算子（HAVING）
    EXPECT_EQ(logical_plan->operator_type(), dreamdb::planner::logical::LogicalOperatorType::Filter);
    EXPECT_EQ(logical_plan->child_count(), 1);

    // 验证子节点是 Aggregate 算子
    const auto & aggregate_operator = logical_plan->child_at(0);
    EXPECT_EQ(aggregate_operator.operator_type(), dreamdb::planner::logical::LogicalOperatorType::Aggregate);
    EXPECT_EQ(aggregate_operator.child_count(), 1);
}

/**
 * @brief 测试计划 SELECT 语句（带 ORDER BY 子句）
 * @details 应该生成 Scan -> Project -> Sort 的计划树
 */
TEST_F(SelectLogicalPlannerTest, PlanSelectWithOrderBy)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 SELECT 语句：SELECT id, name FROM test_collection ORDER BY id
    std::vector<dreamdb::parser::ast::AstSelectItem> select_items;
    auto id_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(std::move(id_column), std::nullopt));
    auto name_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("name", 1, 1);
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(std::move(name_column), std::nullopt));

    // ORDER BY id
    std::vector<dreamdb::parser::ast::AstOrderByItem> order_by;
    auto order_by_id = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    order_by.push_back(dreamdb::parser::ast::AstOrderByItem(std::move(order_by_id), dreamdb::common::Direction::ASC));

    auto ast_select_statement = dreamdb::parser::ast::AstSelectStatement::create(
        "test_collection",
        std::move(select_items),
        nullptr,  // 无 WHERE 子句
        {},       // 无 GROUP BY 子句
        nullptr,  // 无 HAVING 子句
        std::move(order_by),
        std::nullopt,  // 无 LIMIT
        std::nullopt,  // 无 OFFSET
        1, 1
    );

    // 绑定语句
    auto bound_select_statement = binder.bind(*ast_select_statement);
    ASSERT_NE(bound_select_statement, nullptr);

    // 计划语句
    auto logical_plan = planner.plan(*bound_select_statement);
    ASSERT_NE(logical_plan, nullptr);

    // 验证根节点是 Sort 算子
    EXPECT_EQ(logical_plan->operator_type(), dreamdb::planner::logical::LogicalOperatorType::Sort);
    EXPECT_EQ(logical_plan->child_count(), 1);

    // 验证 Sort 算子的属性
    const auto * sort_operator = dynamic_cast<const dreamdb::planner::logical::LogicalSort *>(logical_plan.get());
    ASSERT_NE(sort_operator, nullptr);
    EXPECT_EQ(sort_operator->sort_item_count(), 1);

    // 验证子节点是 Project 算子
    const auto & project_operator = logical_plan->child_at(0);
    EXPECT_EQ(project_operator.operator_type(), dreamdb::planner::logical::LogicalOperatorType::Project);
    EXPECT_EQ(project_operator.child_count(), 1);
}

/**
 * @brief 测试计划 SELECT 语句（带 LIMIT 子句）
 * @details 应该生成 Scan -> Project -> LimitOffset 的计划树
 */
TEST_F(SelectLogicalPlannerTest, PlanSelectWithLimit)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 SELECT 语句：SELECT id FROM test_collection LIMIT 10
    std::vector<dreamdb::parser::ast::AstSelectItem> select_items;
    auto id_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(std::move(id_column), std::nullopt));

    auto ast_select_statement = dreamdb::parser::ast::AstSelectStatement::create(
        "test_collection",
        std::move(select_items),
        nullptr,  // 无 WHERE 子句
        {},       // 无 GROUP BY 子句
        nullptr,  // 无 HAVING 子句
        {},       // 无 ORDER BY 子句
        std::make_optional<std::uint64_t>(10),  // LIMIT 10
        std::nullopt,  // 无 OFFSET
        1, 1
    );

    // 绑定语句
    auto bound_select_statement = binder.bind(*ast_select_statement);
    ASSERT_NE(bound_select_statement, nullptr);

    // 计划语句
    auto logical_plan = planner.plan(*bound_select_statement);
    ASSERT_NE(logical_plan, nullptr);

    // 验证根节点是 LimitOffset 算子
    EXPECT_EQ(logical_plan->operator_type(), dreamdb::planner::logical::LogicalOperatorType::LimitOffset);
    EXPECT_EQ(logical_plan->child_count(), 1);

    // 验证 LimitOffset 算子的属性
    const auto * limit_offset_operator = dynamic_cast<const dreamdb::planner::logical::LogicalLimitOffset *>(logical_plan.get());
    ASSERT_NE(limit_offset_operator, nullptr);
    EXPECT_TRUE(limit_offset_operator->limit().has_value());
    EXPECT_EQ(limit_offset_operator->limit().value(), 10);
    EXPECT_FALSE(limit_offset_operator->offset().has_value());
}

/**
 * @brief 测试计划 SELECT 语句（带 OFFSET 子句）
 * @details 应该生成 Scan -> Project -> LimitOffset 的计划树
 */
TEST_F(SelectLogicalPlannerTest, PlanSelectWithOffset)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 SELECT 语句：SELECT id FROM test_collection OFFSET 5
    std::vector<dreamdb::parser::ast::AstSelectItem> select_items;
    auto id_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(std::move(id_column), std::nullopt));

    auto ast_select_statement = dreamdb::parser::ast::AstSelectStatement::create(
        "test_collection",
        std::move(select_items),
        nullptr,  // 无 WHERE 子句
        {},       // 无 GROUP BY 子句
        nullptr,  // 无 HAVING 子句
        {},       // 无 ORDER BY 子句
        std::nullopt,  // 无 LIMIT
        std::make_optional<std::uint64_t>(5),  // OFFSET 5
        1, 1
    );

    // 绑定语句
    auto bound_select_statement = binder.bind(*ast_select_statement);
    ASSERT_NE(bound_select_statement, nullptr);

    // 计划语句
    auto logical_plan = planner.plan(*bound_select_statement);
    ASSERT_NE(logical_plan, nullptr);

    // 验证根节点是 LimitOffset 算子
    EXPECT_EQ(logical_plan->operator_type(), dreamdb::planner::logical::LogicalOperatorType::LimitOffset);
    EXPECT_EQ(logical_plan->child_count(), 1);

    // 验证 LimitOffset 算子的属性
    const auto * limit_offset_operator = dynamic_cast<const dreamdb::planner::logical::LogicalLimitOffset *>(logical_plan.get());
    ASSERT_NE(limit_offset_operator, nullptr);
    EXPECT_FALSE(limit_offset_operator->limit().has_value());
    EXPECT_TRUE(limit_offset_operator->offset().has_value());
    EXPECT_EQ(limit_offset_operator->offset().value(), 5);
}

/**
 * @brief 测试计划 SELECT 语句（带 LIMIT 和 OFFSET 子句）
 * @details 应该生成 Scan -> Project -> LimitOffset 的计划树
 */
TEST_F(SelectLogicalPlannerTest, PlanSelectWithLimitAndOffset)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 SELECT 语句：SELECT id FROM test_collection LIMIT 10 OFFSET 5
    std::vector<dreamdb::parser::ast::AstSelectItem> select_items;
    auto id_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(std::move(id_column), std::nullopt));

    auto ast_select_statement = dreamdb::parser::ast::AstSelectStatement::create(
        "test_collection",
        std::move(select_items),
        nullptr,  // 无 WHERE 子句
        {},       // 无 GROUP BY 子句
        nullptr,  // 无 HAVING 子句
        {},       // 无 ORDER BY 子句
        std::make_optional<std::uint64_t>(10),  // LIMIT 10
        std::make_optional<std::uint64_t>(5),  // OFFSET 5
        1, 1
    );

    // 绑定语句
    auto bound_select_statement = binder.bind(*ast_select_statement);
    ASSERT_NE(bound_select_statement, nullptr);

    // 计划语句
    auto logical_plan = planner.plan(*bound_select_statement);
    ASSERT_NE(logical_plan, nullptr);

    // 验证根节点是 LimitOffset 算子
    EXPECT_EQ(logical_plan->operator_type(), dreamdb::planner::logical::LogicalOperatorType::LimitOffset);
    EXPECT_EQ(logical_plan->child_count(), 1);

    // 验证 LimitOffset 算子的属性
    const auto * limit_offset_operator = dynamic_cast<const dreamdb::planner::logical::LogicalLimitOffset *>(logical_plan.get());
    ASSERT_NE(limit_offset_operator, nullptr);
    EXPECT_TRUE(limit_offset_operator->limit().has_value());
    EXPECT_EQ(limit_offset_operator->limit().value(), 10);
    EXPECT_TRUE(limit_offset_operator->offset().has_value());
    EXPECT_EQ(limit_offset_operator->offset().value(), 5);
}

/**
 * @brief 测试计划 SELECT 语句（组合场景：WHERE + ORDER BY + LIMIT）
 * @details 应该生成 Scan -> Filter -> Project -> Sort -> LimitOffset 的计划树
 */
TEST_F(SelectLogicalPlannerTest, PlanSelectWithWhereOrderByLimit)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 WHERE 子句：age > 18
    auto age_column_where = dreamdb::parser::ast::AstColumnReferenceExpression::create("age", 1, 1);
    auto literal_eighteen = dreamdb::parser::ast::AstLiteralExpression::create_integer(18, 1, 1);
    auto where_expr = dreamdb::parser::ast::AstBinaryExpression::create(
        dreamdb::parser::ast::AstBinaryOperatorType::GreaterThan,
        std::move(age_column_where),
        std::move(literal_eighteen),
        1, 1
    );

    // 创建 SELECT 语句：SELECT id, name FROM test_collection WHERE age > 18 ORDER BY id LIMIT 10
    std::vector<dreamdb::parser::ast::AstSelectItem> select_items;
    auto id_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(std::move(id_column), std::nullopt));
    auto name_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("name", 1, 1);
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(std::move(name_column), std::nullopt));

    // ORDER BY id
    std::vector<dreamdb::parser::ast::AstOrderByItem> order_by;
    auto order_by_id = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    order_by.push_back(dreamdb::parser::ast::AstOrderByItem(std::move(order_by_id), dreamdb::common::Direction::ASC));

    auto ast_select_statement = dreamdb::parser::ast::AstSelectStatement::create(
        "test_collection",
        std::move(select_items),
        std::move(where_expr),
        {},       // 无 GROUP BY 子句
        nullptr,  // 无 HAVING 子句
        std::move(order_by),
        std::make_optional<std::uint64_t>(10),  // LIMIT 10
        std::nullopt,  // 无 OFFSET
        1, 1
    );

    // 绑定语句
    auto bound_select_statement = binder.bind(*ast_select_statement);
    ASSERT_NE(bound_select_statement, nullptr);

    // 计划语句
    auto logical_plan = planner.plan(*bound_select_statement);
    ASSERT_NE(logical_plan, nullptr);

    // 验证根节点是 LimitOffset 算子
    EXPECT_EQ(logical_plan->operator_type(), dreamdb::planner::logical::LogicalOperatorType::LimitOffset);
    EXPECT_EQ(logical_plan->child_count(), 1);

    // 验证 LimitOffset 的子节点是 Sort 算子
    const auto & sort_operator = logical_plan->child_at(0);
    EXPECT_EQ(sort_operator.operator_type(), dreamdb::planner::logical::LogicalOperatorType::Sort);
    EXPECT_EQ(sort_operator.child_count(), 1);

    // 验证 Sort 的子节点是 Project 算子
    const auto & project_operator = sort_operator.child_at(0);
    EXPECT_EQ(project_operator.operator_type(), dreamdb::planner::logical::LogicalOperatorType::Project);
    EXPECT_EQ(project_operator.child_count(), 1);

    // 验证 Project 的子节点是 Filter 算子
    const auto & filter_operator = project_operator.child_at(0);
    EXPECT_EQ(filter_operator.operator_type(), dreamdb::planner::logical::LogicalOperatorType::Filter);
    EXPECT_EQ(filter_operator.child_count(), 1);

    // 验证 Filter 的子节点是 Scan 算子
    const auto & scan_operator = filter_operator.child_at(0);
    EXPECT_EQ(scan_operator.operator_type(), dreamdb::planner::logical::LogicalOperatorType::Scan);
    const auto * scan = dynamic_cast<const dreamdb::planner::logical::LogicalScan *>(&scan_operator);
    ASSERT_NE(scan, nullptr);
    // WHERE 使用了 age，SELECT 需要 id 和 name，ORDER BY 需要 id
    EXPECT_EQ(scan->column_count(), 3);
}

/**
 * @brief 测试计划 SELECT 语句（组合场景：GROUP BY + HAVING + ORDER BY）
 * @details 应该生成 Scan -> Aggregate -> Filter -> Sort 的计划树
 */
TEST_F(SelectLogicalPlannerTest, PlanSelectWithGroupByHavingOrderBy)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 SELECT 语句：SELECT age, COUNT(*) FROM test_collection GROUP BY age HAVING COUNT(*) > 1 ORDER BY age
    std::vector<dreamdb::parser::ast::AstSelectItem> select_items;
    auto age_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("age", 1, 1);
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(std::move(age_column), std::nullopt));
    
    // COUNT(*) 函数调用（无参数）
    auto count_func = dreamdb::parser::ast::AstFunctionCallExpression::create(
        "COUNT",
        std::vector<std::unique_ptr<dreamdb::parser::ast::AstExpression>>{},
        1, 1
    );
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(std::move(count_func), std::nullopt));

    // GROUP BY age
    std::vector<std::unique_ptr<dreamdb::parser::ast::AstExpression>> group_by;
    auto group_by_age = dreamdb::parser::ast::AstColumnReferenceExpression::create("age", 1, 1);
    group_by.push_back(std::move(group_by_age));

    // HAVING COUNT(*) > 1
    auto having_count_func = dreamdb::parser::ast::AstFunctionCallExpression::create(
        "COUNT",
        std::vector<std::unique_ptr<dreamdb::parser::ast::AstExpression>>{},
        1, 1
    );
    auto literal_one = dreamdb::parser::ast::AstLiteralExpression::create_integer(1, 1, 1);
    auto having_expr = dreamdb::parser::ast::AstBinaryExpression::create(
        dreamdb::parser::ast::AstBinaryOperatorType::GreaterThan,
        std::move(having_count_func),
        std::move(literal_one),
        1, 1
    );

    // ORDER BY age
    std::vector<dreamdb::parser::ast::AstOrderByItem> order_by;
    auto order_by_age = dreamdb::parser::ast::AstColumnReferenceExpression::create("age", 1, 1);
    order_by.push_back(dreamdb::parser::ast::AstOrderByItem(std::move(order_by_age), dreamdb::common::Direction::ASC));

    auto ast_select_statement = dreamdb::parser::ast::AstSelectStatement::create(
        "test_collection",
        std::move(select_items),
        nullptr,  // 无 WHERE 子句
        std::move(group_by),
        std::move(having_expr),
        std::move(order_by),
        std::nullopt,  // 无 LIMIT
        std::nullopt,  // 无 OFFSET
        1, 1
    );

    // 绑定语句
    auto bound_select_statement = binder.bind(*ast_select_statement);
    ASSERT_NE(bound_select_statement, nullptr);

    // 计划语句
    auto logical_plan = planner.plan(*bound_select_statement);
    ASSERT_NE(logical_plan, nullptr);

    // 验证根节点是 Sort 算子
    EXPECT_EQ(logical_plan->operator_type(), dreamdb::planner::logical::LogicalOperatorType::Sort);
    EXPECT_EQ(logical_plan->child_count(), 1);

    // 验证 Sort 的子节点是 Filter 算子（HAVING）
    const auto & filter_operator = logical_plan->child_at(0);
    EXPECT_EQ(filter_operator.operator_type(), dreamdb::planner::logical::LogicalOperatorType::Filter);
    EXPECT_EQ(filter_operator.child_count(), 1);

    // 验证 Filter 的子节点是 Aggregate 算子
    const auto & aggregate_operator = filter_operator.child_at(0);
    EXPECT_EQ(aggregate_operator.operator_type(), dreamdb::planner::logical::LogicalOperatorType::Aggregate);
    EXPECT_EQ(aggregate_operator.child_count(), 1);
}