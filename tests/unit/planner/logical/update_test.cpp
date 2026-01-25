#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <dreamdb/binder/binder.h>
#include <dreamdb/binder/bound/statement/update.h>
#include <dreamdb/parser/ast/statement/use.h>
#include <dreamdb/parser/ast/statement/update.h>
#include <dreamdb/parser/ast/expression/literal.h>
#include <dreamdb/parser/ast/expression/column_reference.h>
#include <dreamdb/parser/ast/expression/binary.h>
#include <dreamdb/planner/logical/logical_planner.h>
#include <dreamdb/planner/logical/operator/logical_scan.h>
#include <dreamdb/planner/logical/operator/logical_filter.h>
#include <dreamdb/planner/logical/operator/logical_update.h>

#include "../../binder/catalog_mock.h"

using ::testing::Return;

class UpdateLogicalPlannerTest : public ::testing::Test
{
public:
    UpdateLogicalPlannerTest()
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

        // 设置列解析
        dreamdb::catalog::column_info_t id_column_info{
            dreamdb::common::column_id_t{0},
            0,  // index
            "id",
            dreamdb::common::LogicalType{dreamdb::common::LogicalTypeId::Integer},
            true,  // is_nullable
            true   // is_insertable
        };
        ON_CALL(catalog_mock, resolve_column(
            dreamdb::common::collection_id_t{99}, "id"
        ))
            .WillByDefault(Return(std::make_optional<dreamdb::catalog::column_info_t>(id_column_info)));

        dreamdb::catalog::column_info_t name_column_info{
            dreamdb::common::column_id_t{1},
            1,  // index
            "name",
            dreamdb::common::LogicalType{dreamdb::common::LogicalTypeId::String},
            true,  // is_nullable
            true   // is_insertable
        };
        ON_CALL(catalog_mock, resolve_column(
            dreamdb::common::collection_id_t{99}, "name"
        ))
            .WillByDefault(Return(std::make_optional<dreamdb::catalog::column_info_t>(name_column_info)));

        dreamdb::catalog::column_info_t age_column_info{
            dreamdb::common::column_id_t{2},
            2,  // index
            "age",
            dreamdb::common::LogicalType{dreamdb::common::LogicalTypeId::Integer},
            true,  // is_nullable
            true   // is_insertable
        };
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
 * @brief 测试计划 UPDATE 语句（无 WHERE 子句，单列更新）
 * @details 应该生成 Scan -> Update 的计划树
 */
TEST_F(UpdateLogicalPlannerTest, PlanUpdateWithoutWhere)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 UPDATE 语句：SET name = 'new_name'
    std::vector<dreamdb::parser::ast::AstUpdateAssignment> assignments;
    auto literal_new_name = dreamdb::parser::ast::AstLiteralExpression::create_string("new_name", 1, 1);
    assignments.emplace_back("name", std::move(literal_new_name));

    auto ast_update_statement = dreamdb::parser::ast::AstUpdateStatement::create(
        "test_collection",
        std::move(assignments),
        nullptr,  // 无 WHERE 子句
        1, 1
    );

    // 绑定语句
    auto bound_update_statement = binder.bind(*ast_update_statement);
    ASSERT_NE(bound_update_statement, nullptr);

    // 计划语句
    auto logical_plan = planner.plan(*bound_update_statement);
    ASSERT_NE(logical_plan, nullptr);

    // 验证根节点是 Update 算子
    EXPECT_EQ(logical_plan->operator_type(), dreamdb::planner::logical::LogicalOperatorType::Update);
    EXPECT_EQ(logical_plan->child_count(), 1);

    // 验证 Update 算子的属性
    const auto * update_operator = dynamic_cast<const dreamdb::planner::logical::LogicalUpdate *>(logical_plan.get());
    ASSERT_NE(update_operator, nullptr);
    EXPECT_EQ(update_operator->collection_id(), 99);
    EXPECT_EQ(update_operator->update_item_count(), 1);
    EXPECT_EQ(update_operator->update_item_at(0).column_id, dreamdb::common::column_id_t{1});  // name 列的 ID 是 1

    // 验证子节点是 Scan 算子
    const auto & scan_operator = logical_plan->child_at(0);
    EXPECT_EQ(scan_operator.operator_type(), dreamdb::planner::logical::LogicalOperatorType::Scan);
    EXPECT_EQ(scan_operator.child_count(), 0);

    const auto * scan = dynamic_cast<const dreamdb::planner::logical::LogicalScan *>(&scan_operator);
    ASSERT_NE(scan, nullptr);
    EXPECT_EQ(scan->collection_id(), 99);
    EXPECT_EQ(scan->column_count(), 1);  // 需要扫描 name 列以便更新
    EXPECT_EQ(scan->column_at(0), dreamdb::common::column_id_t{1});  // name 列
}

/**
 * @brief 测试计划 UPDATE 语句（带简单 WHERE 子句）
 * @details 应该生成 Scan -> Filter -> Update 的计划树
 */
TEST_F(UpdateLogicalPlannerTest, PlanUpdateWithSimpleWhere)
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

    // 创建 UPDATE 语句：SET name = 'new_name' WHERE id = 1
    std::vector<dreamdb::parser::ast::AstUpdateAssignment> assignments;
    auto literal_new_name = dreamdb::parser::ast::AstLiteralExpression::create_string("new_name", 1, 1);
    assignments.emplace_back("name", std::move(literal_new_name));

    auto ast_update_statement = dreamdb::parser::ast::AstUpdateStatement::create(
        "test_collection",
        std::move(assignments),
        std::move(where_expr),
        1, 1
    );

    // 绑定语句
    auto bound_update_statement = binder.bind(*ast_update_statement);
    ASSERT_NE(bound_update_statement, nullptr);

    // 计划语句
    auto logical_plan = planner.plan(*bound_update_statement);
    ASSERT_NE(logical_plan, nullptr);

    // 验证根节点是 Update 算子
    EXPECT_EQ(logical_plan->operator_type(), dreamdb::planner::logical::LogicalOperatorType::Update);
    EXPECT_EQ(logical_plan->child_count(), 1);

    // 验证 Update 算子的属性
    const auto * update_operator = dynamic_cast<const dreamdb::planner::logical::LogicalUpdate *>(logical_plan.get());
    ASSERT_NE(update_operator, nullptr);
    EXPECT_EQ(update_operator->collection_id(), 99);
    EXPECT_EQ(update_operator->update_item_count(), 1);

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
    // WHERE 子句使用了 id 列，UPDATE 需要 name 列
    EXPECT_EQ(scan->column_count(), 2);
    // 列 ID 应该是有序的（去重后）
    EXPECT_EQ(scan->column_at(0), dreamdb::common::column_id_t{0});  // id
    EXPECT_EQ(scan->column_at(1), dreamdb::common::column_id_t{1});  // name
}

/**
 * @brief 测试计划 UPDATE 语句（带复杂 WHERE 子句）
 * @details 应该生成 Scan -> Filter -> Update 的计划树，Scan 应该包含所有需要的列
 */
TEST_F(UpdateLogicalPlannerTest, PlanUpdateWithComplexWhere)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 WHERE 子句：id = 1 AND age > 18
    auto id_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    auto literal_one = dreamdb::parser::ast::AstLiteralExpression::create_integer(1, 1, 1);
    auto id_eq_one = dreamdb::parser::ast::AstBinaryExpression::create(
        dreamdb::parser::ast::AstBinaryOperatorType::Equal,
        std::move(id_column),
        std::move(literal_one),
        1, 1
    );

    auto age_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("age", 1, 1);
    auto literal_eighteen = dreamdb::parser::ast::AstLiteralExpression::create_integer(18, 1, 1);
    auto age_gt_eighteen = dreamdb::parser::ast::AstBinaryExpression::create(
        dreamdb::parser::ast::AstBinaryOperatorType::GreaterThan,
        std::move(age_column),
        std::move(literal_eighteen),
        1, 1
    );

    auto where_expr = dreamdb::parser::ast::AstBinaryExpression::create(
        dreamdb::parser::ast::AstBinaryOperatorType::And,
        std::move(id_eq_one),
        std::move(age_gt_eighteen),
        1, 1
    );

    // 创建 UPDATE 语句：SET name = 'new_name' WHERE id = 1 AND age > 18
    std::vector<dreamdb::parser::ast::AstUpdateAssignment> assignments;
    auto literal_new_name = dreamdb::parser::ast::AstLiteralExpression::create_string("new_name", 1, 1);
    assignments.emplace_back("name", std::move(literal_new_name));

    auto ast_update_statement = dreamdb::parser::ast::AstUpdateStatement::create(
        "test_collection",
        std::move(assignments),
        std::move(where_expr),
        1, 1
    );

    // 绑定语句
    auto bound_update_statement = binder.bind(*ast_update_statement);
    ASSERT_NE(bound_update_statement, nullptr);

    // 计划语句
    auto logical_plan = planner.plan(*bound_update_statement);
    ASSERT_NE(logical_plan, nullptr);

    // 验证 Scan 算子包含所有需要的列
    const auto & filter_operator = logical_plan->child_at(0);
    const auto & scan_operator = filter_operator.child_at(0);
    const auto * scan = dynamic_cast<const dreamdb::planner::logical::LogicalScan *>(&scan_operator);
    ASSERT_NE(scan, nullptr);
    // WHERE 子句使用了 id 和 age 两列，UPDATE 需要 name 列
    EXPECT_EQ(scan->column_count(), 3);
    // 列 ID 应该是有序的（去重后）
    EXPECT_EQ(scan->column_at(0), dreamdb::common::column_id_t{0});  // id
    EXPECT_EQ(scan->column_at(1), dreamdb::common::column_id_t{1});  // name
    EXPECT_EQ(scan->column_at(2), dreamdb::common::column_id_t{2});  // age
}

/**
 * @brief 测试计划 UPDATE 语句（更新多个列）
 * @details Scan 应该包含所有需要更新的列
 */
TEST_F(UpdateLogicalPlannerTest, PlanUpdateWithMultipleColumns)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 UPDATE 语句：SET name = 'new_name', age = 25
    std::vector<dreamdb::parser::ast::AstUpdateAssignment> assignments;
    auto literal_new_name = dreamdb::parser::ast::AstLiteralExpression::create_string("new_name", 1, 1);
    assignments.emplace_back("name", std::move(literal_new_name));
    auto literal_twenty_five = dreamdb::parser::ast::AstLiteralExpression::create_integer(25, 1, 1);
    assignments.emplace_back("age", std::move(literal_twenty_five));

    auto ast_update_statement = dreamdb::parser::ast::AstUpdateStatement::create(
        "test_collection",
        std::move(assignments),
        nullptr,  // 无 WHERE 子句
        1, 1
    );

    // 绑定语句
    auto bound_update_statement = binder.bind(*ast_update_statement);
    ASSERT_NE(bound_update_statement, nullptr);

    // 计划语句
    auto logical_plan = planner.plan(*bound_update_statement);
    ASSERT_NE(logical_plan, nullptr);

    // 验证 Update 算子包含两个更新项
    const auto * update_operator = dynamic_cast<const dreamdb::planner::logical::LogicalUpdate *>(logical_plan.get());
    ASSERT_NE(update_operator, nullptr);
    EXPECT_EQ(update_operator->update_item_count(), 2);

    // 验证 Scan 算子包含所有需要更新的列
    const auto & scan_operator = logical_plan->child_at(0);
    const auto * scan = dynamic_cast<const dreamdb::planner::logical::LogicalScan *>(&scan_operator);
    ASSERT_NE(scan, nullptr);
    EXPECT_EQ(scan->column_count(), 2);  // name 和 age 两列
    EXPECT_EQ(scan->column_at(0), dreamdb::common::column_id_t{1});  // name
    EXPECT_EQ(scan->column_at(1), dreamdb::common::column_id_t{2});  // age
}

/**
 * @brief 测试计划 UPDATE 语句（更新值中包含列引用）
 * @details 例如 SET age = age + 1，Scan 应该包含 age 列
 */
TEST_F(UpdateLogicalPlannerTest, PlanUpdateWithColumnInValue)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 UPDATE 语句：SET age = age + 1
    std::vector<dreamdb::parser::ast::AstUpdateAssignment> assignments;
    auto age_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("age", 1, 1);
    auto literal_one = dreamdb::parser::ast::AstLiteralExpression::create_integer(1, 1, 1);
    auto age_plus_one = dreamdb::parser::ast::AstBinaryExpression::create(
        dreamdb::parser::ast::AstBinaryOperatorType::Plus,
        std::move(age_column),
        std::move(literal_one),
        1, 1
    );
    assignments.emplace_back("age", std::move(age_plus_one));

    auto ast_update_statement = dreamdb::parser::ast::AstUpdateStatement::create(
        "test_collection",
        std::move(assignments),
        nullptr,  // 无 WHERE 子句
        1, 1
    );

    // 绑定语句
    auto bound_update_statement = binder.bind(*ast_update_statement);
    ASSERT_NE(bound_update_statement, nullptr);

    // 计划语句
    auto logical_plan = planner.plan(*bound_update_statement);
    ASSERT_NE(logical_plan, nullptr);

    // 验证 Scan 算子包含 age 列（既用于更新，也在值表达式中使用）
    const auto & scan_operator = logical_plan->child_at(0);
    const auto * scan = dynamic_cast<const dreamdb::planner::logical::LogicalScan *>(&scan_operator);
    ASSERT_NE(scan, nullptr);
    EXPECT_EQ(scan->column_count(), 1);  // age 列（去重后）
    EXPECT_EQ(scan->column_at(0), dreamdb::common::column_id_t{2});  // age
}

/**
 * @brief 测试计划 UPDATE 语句（WHERE 子句和更新值都使用同一列）
 * @details Scan 应该只包含去重后的列 ID
 */
TEST_F(UpdateLogicalPlannerTest, PlanUpdateWithDuplicateColumns)
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

    // 创建 UPDATE 语句：SET age = age + 1 WHERE age > 18
    std::vector<dreamdb::parser::ast::AstUpdateAssignment> assignments;
    auto age_column_value = dreamdb::parser::ast::AstColumnReferenceExpression::create("age", 1, 1);
    auto literal_one = dreamdb::parser::ast::AstLiteralExpression::create_integer(1, 1, 1);
    auto age_plus_one = dreamdb::parser::ast::AstBinaryExpression::create(
        dreamdb::parser::ast::AstBinaryOperatorType::Plus,
        std::move(age_column_value),
        std::move(literal_one),
        1, 1
    );
    assignments.emplace_back("age", std::move(age_plus_one));

    auto ast_update_statement = dreamdb::parser::ast::AstUpdateStatement::create(
        "test_collection",
        std::move(assignments),
        std::move(where_expr),
        1, 1
    );

    // 绑定语句
    auto bound_update_statement = binder.bind(*ast_update_statement);
    ASSERT_NE(bound_update_statement, nullptr);

    // 计划语句
    auto logical_plan = planner.plan(*bound_update_statement);
    ASSERT_NE(logical_plan, nullptr);

    // 验证 Scan 算子只包含一个列（age 列去重后）
    const auto & filter_operator = logical_plan->child_at(0);
    const auto & scan_operator = filter_operator.child_at(0);
    const auto * scan = dynamic_cast<const dreamdb::planner::logical::LogicalScan *>(&scan_operator);
    ASSERT_NE(scan, nullptr);
    EXPECT_EQ(scan->column_count(), 1);  // age 列只出现一次（去重）
    EXPECT_EQ(scan->column_at(0), dreamdb::common::column_id_t{2});  // age
}
