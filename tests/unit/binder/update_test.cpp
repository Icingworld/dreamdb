#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <dreamdb/binder/binder.h>
#include <dreamdb/binder/bound/debug/debug_printer.h>
#include <dreamdb/binder/bound/statement/statement.h>
#include <dreamdb/binder/bound/statement/update.h>
#include <dreamdb/parser/ast/statement/use.h>
#include <dreamdb/parser/ast/statement/update.h>
#include <dreamdb/parser/ast/expression/literal.h>
#include <dreamdb/parser/ast/expression/column_reference.h>
#include <dreamdb/parser/ast/expression/binary.h>

#include "catalog_mock.h"

using ::testing::Return;

class UpdateBinderTest : public ::testing::Test
{
public:
    UpdateBinderTest()
        : catalog_mock()
        , binder(catalog_mock)
        , printer()
    {
        // 默认行为：解析数据库与集合
        ON_CALL(catalog_mock, resolve_database("test_db"))
            .WillByDefault(Return(std::make_optional<dreamdb::common::database_id_t>(12345)));

        ON_CALL(catalog_mock, resolve_collection(
            dreamdb::common::database_id_t{12345}, "test_collection"
        ))
            .WillByDefault(Return(std::make_optional<dreamdb::common::collection_id_t>(99)));

        ON_CALL(catalog_mock, resolve_collection(
            dreamdb::common::database_id_t{12345}, "nonexistent_collection"
        ))
            .WillByDefault(Return(std::nullopt));

        ON_CALL(catalog_mock, resolve_collection(
            dreamdb::common::database_id_t{12345}, "another_collection"
        ))
            .WillByDefault(Return(std::make_optional<dreamdb::common::collection_id_t>(100)));

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

        ON_CALL(catalog_mock, resolve_column(
            dreamdb::common::collection_id_t{99}, "nonexistent_column"
        ))
            .WillByDefault(Return(std::nullopt));
    }

public:
    CatalogMock catalog_mock;
    dreamdb::binder::Binder binder;
    dreamdb::binder::bound::BoundDebugPrinter printer;
};

/**
 * @brief 测试绑定 UPDATE 语句（无 WHERE 子句，单个赋值）
 */
TEST_F(UpdateBinderTest, BindUpdateWithoutWhere)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建赋值项：name = 'updated'
    std::vector<dreamdb::parser::ast::AstUpdateAssignment> assignments;
    auto literal_value = dreamdb::parser::ast::AstLiteralExpression::create_string("updated", 1, 1);
    std::unique_ptr<dreamdb::parser::ast::AstExpression> expr = std::move(literal_value);
    assignments.push_back(dreamdb::parser::ast::AstUpdateAssignment("name", std::move(expr)));

    // 创建 UPDATE 语句（无 WHERE 子句）
    auto ast_update_statement = dreamdb::parser::ast::AstUpdateStatement::create(
        "test_collection",
        std::move(assignments),
        nullptr,  // 无 WHERE 子句
        1, 1
    );

    // 绑定语句
    auto bound_update_statement = binder.bind(*ast_update_statement);
    ASSERT_NE(bound_update_statement, nullptr);

    // 验证语句类型
    EXPECT_EQ(bound_update_statement->statement_type(), dreamdb::binder::bound::BoundStatementType::Update);

    // 验证 BoundUpdateStatement 的内容
    auto * update_stmt = dynamic_cast<const dreamdb::binder::bound::BoundUpdateStatement *>(bound_update_statement.get());
    ASSERT_NE(update_stmt, nullptr);
    EXPECT_EQ(update_stmt->collection_id(), 99);
    EXPECT_EQ(update_stmt->update_item_count(), 1);
    EXPECT_FALSE(update_stmt->has_where());

    // 验证格式化输出
    std::string formatted = printer.format(*bound_update_statement);
    EXPECT_TRUE(formatted.find("UPDATE collection_id:99") != std::string::npos);
    EXPECT_TRUE(formatted.find("SET") != std::string::npos);
}

/**
 * @brief 测试绑定 UPDATE 语句（带 WHERE 子句）
 */
TEST_F(UpdateBinderTest, BindUpdateWithWhere)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建赋值项：name = 'updated'
    std::vector<dreamdb::parser::ast::AstUpdateAssignment> assignments;
    auto literal_value = dreamdb::parser::ast::AstLiteralExpression::create_string("updated", 1, 1);
    std::unique_ptr<dreamdb::parser::ast::AstExpression> expr = std::move(literal_value);
    assignments.push_back(dreamdb::parser::ast::AstUpdateAssignment("name", std::move(expr)));

    // 创建 WHERE 子句：id = 1
    auto id_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    auto literal_one = dreamdb::parser::ast::AstLiteralExpression::create_integer(1, 1, 1);
    auto where_expr = dreamdb::parser::ast::AstBinaryExpression::create(
        dreamdb::parser::ast::AstBinaryOperatorType::Equal,
        std::move(id_column),
        std::move(literal_one),
        1, 1
    );

    // 创建 UPDATE 语句（带 WHERE 子句）
    auto ast_update_statement = dreamdb::parser::ast::AstUpdateStatement::create(
        "test_collection",
        std::move(assignments),
        std::move(where_expr),
        1, 1
    );

    // 绑定语句
    auto bound_update_statement = binder.bind(*ast_update_statement);
    ASSERT_NE(bound_update_statement, nullptr);

    // 验证语句类型
    EXPECT_EQ(bound_update_statement->statement_type(), dreamdb::binder::bound::BoundStatementType::Update);

    // 验证 BoundUpdateStatement 的内容
    auto * update_stmt = dynamic_cast<const dreamdb::binder::bound::BoundUpdateStatement *>(bound_update_statement.get());
    ASSERT_NE(update_stmt, nullptr);
    EXPECT_EQ(update_stmt->collection_id(), 99);
    EXPECT_EQ(update_stmt->update_item_count(), 1);
    EXPECT_TRUE(update_stmt->has_where());
    ASSERT_NE(update_stmt->where(), nullptr);

    // 验证格式化输出
    std::string formatted = printer.format(*bound_update_statement);
    EXPECT_TRUE(formatted.find("UPDATE collection_id:99") != std::string::npos);
    EXPECT_TRUE(formatted.find("SET") != std::string::npos);
    EXPECT_TRUE(formatted.find("WHERE") != std::string::npos);
}

/**
 * @brief 测试绑定 UPDATE 语句（多个赋值项）
 */
TEST_F(UpdateBinderTest, BindUpdateMultipleAssignments)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建多个赋值项：name = 'updated', age = 25
    std::vector<dreamdb::parser::ast::AstUpdateAssignment> assignments;
    auto name_value = dreamdb::parser::ast::AstLiteralExpression::create_string("updated", 1, 1);
    std::unique_ptr<dreamdb::parser::ast::AstExpression> name_expr = std::move(name_value);
    assignments.push_back(dreamdb::parser::ast::AstUpdateAssignment("name", std::move(name_expr)));
    auto age_value = dreamdb::parser::ast::AstLiteralExpression::create_integer(25, 1, 1);
    std::unique_ptr<dreamdb::parser::ast::AstExpression> age_expr = std::move(age_value);
    assignments.push_back(dreamdb::parser::ast::AstUpdateAssignment("age", std::move(age_expr)));

    // 创建 UPDATE 语句
    auto ast_update_statement = dreamdb::parser::ast::AstUpdateStatement::create(
        "test_collection",
        std::move(assignments),
        nullptr,
        1, 1
    );

    // 绑定语句
    auto bound_update_statement = binder.bind(*ast_update_statement);
    ASSERT_NE(bound_update_statement, nullptr);

    // 验证 BoundUpdateStatement 的内容
    auto * update_stmt = dynamic_cast<const dreamdb::binder::bound::BoundUpdateStatement *>(bound_update_statement.get());
    ASSERT_NE(update_stmt, nullptr);
    EXPECT_EQ(update_stmt->collection_id(), 99);
    EXPECT_EQ(update_stmt->update_item_count(), 2);
    EXPECT_FALSE(update_stmt->has_where());
}

/**
 * @brief 测试绑定不同的集合
 */
TEST_F(UpdateBinderTest, BindUpdateDifferentCollection)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建赋值项
    std::vector<dreamdb::parser::ast::AstUpdateAssignment> assignments;
    auto literal_value = dreamdb::parser::ast::AstLiteralExpression::create_string("updated", 1, 1);
    std::unique_ptr<dreamdb::parser::ast::AstExpression> expr = std::move(literal_value);
    assignments.push_back(dreamdb::parser::ast::AstUpdateAssignment("name", std::move(expr)));

    // 创建 UPDATE 语句（不同的集合）
    auto ast_update_statement = dreamdb::parser::ast::AstUpdateStatement::create(
        "another_collection",
        std::move(assignments),
        nullptr,
        1, 1
    );

    // 绑定语句应该抛出异常（因为 another_collection 没有设置列解析）
    EXPECT_THROW(
        {
            auto bound_update_statement = binder.bind(*ast_update_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试集合不存在的情况
 */
TEST_F(UpdateBinderTest, BindUpdateNonexistentCollection)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建赋值项
    std::vector<dreamdb::parser::ast::AstUpdateAssignment> assignments;
    auto literal_value = dreamdb::parser::ast::AstLiteralExpression::create_string("updated", 1, 1);
    std::unique_ptr<dreamdb::parser::ast::AstExpression> expr = std::move(literal_value);
    assignments.push_back(dreamdb::parser::ast::AstUpdateAssignment("name", std::move(expr)));

    // 创建 UPDATE 语句（不存在的集合）
    auto ast_update_statement = dreamdb::parser::ast::AstUpdateStatement::create(
        "nonexistent_collection",
        std::move(assignments),
        nullptr,
        1, 1
    );

    // 绑定语句应该抛出异常
    EXPECT_THROW(
        {
            auto bound_update_statement = binder.bind(*ast_update_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试未设置当前数据库的情况
 */
TEST_F(UpdateBinderTest, BindUpdateWithoutCurrentDatabase)
{
    // 不绑定 USE，直接尝试绑定 UPDATE
    std::vector<dreamdb::parser::ast::AstUpdateAssignment> assignments;
    auto literal_value = dreamdb::parser::ast::AstLiteralExpression::create_string("updated", 1, 1);
    std::unique_ptr<dreamdb::parser::ast::AstExpression> expr = std::move(literal_value);
    assignments.push_back(dreamdb::parser::ast::AstUpdateAssignment("name", std::move(expr)));

    auto ast_update_statement = dreamdb::parser::ast::AstUpdateStatement::create(
        "test_collection",
        std::move(assignments),
        nullptr,
        1, 1
    );

    // 绑定语句应该抛出异常
    EXPECT_THROW(
        {
            auto bound_update_statement = binder.bind(*ast_update_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试不存在的列
 */
TEST_F(UpdateBinderTest, BindUpdateNonexistentColumn)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建赋值项（不存在的列）
    std::vector<dreamdb::parser::ast::AstUpdateAssignment> assignments;
    auto literal_value = dreamdb::parser::ast::AstLiteralExpression::create_string("updated", 1, 1);
    std::unique_ptr<dreamdb::parser::ast::AstExpression> expr = std::move(literal_value);
    assignments.push_back(dreamdb::parser::ast::AstUpdateAssignment("nonexistent_column", std::move(expr)));

    // 创建 UPDATE 语句
    auto ast_update_statement = dreamdb::parser::ast::AstUpdateStatement::create(
        "test_collection",
        std::move(assignments),
        nullptr,
        1, 1
    );

    // 绑定语句应该抛出异常
    EXPECT_THROW(
        {
            auto bound_update_statement = binder.bind(*ast_update_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试重复列的情况
 */
TEST_F(UpdateBinderTest, BindUpdateDuplicateColumn)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建赋值项（重复的列）
    std::vector<dreamdb::parser::ast::AstUpdateAssignment> assignments;
    auto value1 = dreamdb::parser::ast::AstLiteralExpression::create_string("value1", 1, 1);
    std::unique_ptr<dreamdb::parser::ast::AstExpression> expr1 = std::move(value1);
    assignments.push_back(dreamdb::parser::ast::AstUpdateAssignment("name", std::move(expr1)));
    auto value2 = dreamdb::parser::ast::AstLiteralExpression::create_string("value2", 1, 1);
    std::unique_ptr<dreamdb::parser::ast::AstExpression> expr2 = std::move(value2);
    assignments.push_back(dreamdb::parser::ast::AstUpdateAssignment("name", std::move(expr2)));  // 重复的列

    // 创建 UPDATE 语句
    auto ast_update_statement = dreamdb::parser::ast::AstUpdateStatement::create(
        "test_collection",
        std::move(assignments),
        nullptr,
        1, 1
    );

    // 绑定语句应该抛出异常
    EXPECT_THROW(
        {
            auto bound_update_statement = binder.bind(*ast_update_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试绑定 UPDATE 语句（带复杂 WHERE 子句）
 */
TEST_F(UpdateBinderTest, BindUpdateWithComplexWhere)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建赋值项：name = 'updated'
    std::vector<dreamdb::parser::ast::AstUpdateAssignment> assignments;
    auto literal_value = dreamdb::parser::ast::AstLiteralExpression::create_string("updated", 1, 1);
    std::unique_ptr<dreamdb::parser::ast::AstExpression> expr = std::move(literal_value);
    assignments.push_back(dreamdb::parser::ast::AstUpdateAssignment("name", std::move(expr)));

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

    // 创建 UPDATE 语句（带复杂 WHERE 子句）
    auto ast_update_statement = dreamdb::parser::ast::AstUpdateStatement::create(
        "test_collection",
        std::move(assignments),
        std::move(where_expr),
        1, 1
    );

    // 绑定语句
    auto bound_update_statement = binder.bind(*ast_update_statement);
    ASSERT_NE(bound_update_statement, nullptr);

    // 验证 BoundUpdateStatement 的内容
    auto * update_stmt = dynamic_cast<const dreamdb::binder::bound::BoundUpdateStatement *>(bound_update_statement.get());
    ASSERT_NE(update_stmt, nullptr);
    EXPECT_EQ(update_stmt->collection_id(), 99);
    EXPECT_EQ(update_stmt->update_item_count(), 1);
    EXPECT_TRUE(update_stmt->has_where());
    ASSERT_NE(update_stmt->where(), nullptr);

    // 验证格式化输出
    std::string formatted = printer.format(*bound_update_statement);
    EXPECT_TRUE(formatted.find("UPDATE collection_id:99") != std::string::npos);
    EXPECT_TRUE(formatted.find("SET") != std::string::npos);
    EXPECT_TRUE(formatted.find("WHERE") != std::string::npos);
}
