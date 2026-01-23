#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <dreamdb/binder/binder.h>
#include <dreamdb/binder/bound/debug/debug_printer.h>
#include <dreamdb/binder/bound/statement/statement.h>
#include <dreamdb/binder/bound/statement/delete.h>
#include <dreamdb/parser/ast/statement/use.h>
#include <dreamdb/parser/ast/statement/delete.h>
#include <dreamdb/parser/ast/expression/literal.h>
#include <dreamdb/parser/ast/expression/column_reference.h>
#include <dreamdb/parser/ast/expression/binary.h>

#include "catalog_mock.h"

using ::testing::Return;

class DeleteBinderTest : public ::testing::Test
{
public:
    DeleteBinderTest()
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
 * @brief 测试绑定 DELETE 语句（无 WHERE 子句）
 */
TEST_F(DeleteBinderTest, BindDeleteWithoutWhere)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 DELETE 语句（无 WHERE 子句）
    auto ast_delete_statement = dreamdb::parser::ast::AstDeleteStatement::create(
        "test_collection",
        nullptr,  // 无 WHERE 子句
        1, 1
    );

    // 绑定语句
    auto bound_delete_statement = binder.bind(*ast_delete_statement);
    ASSERT_NE(bound_delete_statement, nullptr);

    // 验证语句类型
    EXPECT_EQ(bound_delete_statement->statement_type(), dreamdb::binder::bound::BoundStatementType::Delete);

    // 验证 BoundDeleteStatement 的内容
    auto * delete_stmt = dynamic_cast<const dreamdb::binder::bound::BoundDeleteStatement *>(bound_delete_statement.get());
    ASSERT_NE(delete_stmt, nullptr);
    EXPECT_EQ(delete_stmt->collection_id(), 99);
    EXPECT_FALSE(delete_stmt->has_where());

    // 验证格式化输出
    EXPECT_EQ(printer.format(*bound_delete_statement), "DELETE FROM collection_id:99");
}

/**
 * @brief 测试绑定 DELETE 语句（带 WHERE 子句）
 */
TEST_F(DeleteBinderTest, BindDeleteWithWhere)
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

    // 创建 DELETE 语句（带 WHERE 子句）
    auto ast_delete_statement = dreamdb::parser::ast::AstDeleteStatement::create(
        "test_collection",
        std::move(where_expr),
        1, 1
    );

    // 绑定语句
    auto bound_delete_statement = binder.bind(*ast_delete_statement);
    ASSERT_NE(bound_delete_statement, nullptr);

    // 验证语句类型
    EXPECT_EQ(bound_delete_statement->statement_type(), dreamdb::binder::bound::BoundStatementType::Delete);

    // 验证 BoundDeleteStatement 的内容
    auto * delete_stmt = dynamic_cast<const dreamdb::binder::bound::BoundDeleteStatement *>(bound_delete_statement.get());
    ASSERT_NE(delete_stmt, nullptr);
    EXPECT_EQ(delete_stmt->collection_id(), 99);
    EXPECT_TRUE(delete_stmt->has_where());
    ASSERT_NE(delete_stmt->where(), nullptr);

    // 验证格式化输出（WHERE 子句应该被格式化）
    std::string formatted = printer.format(*bound_delete_statement);
    EXPECT_TRUE(formatted.find("DELETE FROM collection_id:99") != std::string::npos);
    EXPECT_TRUE(formatted.find("WHERE") != std::string::npos);
}

/**
 * @brief 测试绑定不同的集合
 */
TEST_F(DeleteBinderTest, BindDeleteDifferentCollection)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 DELETE 语句（不同的集合）
    auto ast_delete_statement = dreamdb::parser::ast::AstDeleteStatement::create(
        "another_collection",
        nullptr,
        1, 1
    );

    // 绑定语句
    auto bound_delete_statement = binder.bind(*ast_delete_statement);
    ASSERT_NE(bound_delete_statement, nullptr);

    // 验证 BoundDeleteStatement 的内容
    auto * delete_stmt = dynamic_cast<const dreamdb::binder::bound::BoundDeleteStatement *>(bound_delete_statement.get());
    ASSERT_NE(delete_stmt, nullptr);
    EXPECT_EQ(delete_stmt->collection_id(), 100);
    EXPECT_FALSE(delete_stmt->has_where());
}

/**
 * @brief 测试集合不存在的情况
 */
TEST_F(DeleteBinderTest, BindDeleteNonexistentCollection)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 DELETE 语句（不存在的集合）
    auto ast_delete_statement = dreamdb::parser::ast::AstDeleteStatement::create(
        "nonexistent_collection",
        nullptr,
        1, 1
    );

    // 绑定语句应该抛出异常
    EXPECT_THROW(
        {
            auto bound_delete_statement = binder.bind(*ast_delete_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试未设置当前数据库的情况
 */
TEST_F(DeleteBinderTest, BindDeleteWithoutCurrentDatabase)
{
    // 不绑定 USE，直接尝试绑定 DELETE
    auto ast_delete_statement = dreamdb::parser::ast::AstDeleteStatement::create(
        "test_collection",
        nullptr,
        1, 1
    );

    // 绑定语句应该抛出异常
    EXPECT_THROW(
        {
            auto bound_delete_statement = binder.bind(*ast_delete_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试绑定 DELETE 语句（带复杂 WHERE 子句）
 */
TEST_F(DeleteBinderTest, BindDeleteWithComplexWhere)
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

    // 创建 DELETE 语句（带复杂 WHERE 子句）
    auto ast_delete_statement = dreamdb::parser::ast::AstDeleteStatement::create(
        "test_collection",
        std::move(where_expr),
        1, 1
    );

    // 绑定语句
    auto bound_delete_statement = binder.bind(*ast_delete_statement);
    ASSERT_NE(bound_delete_statement, nullptr);

    // 验证 BoundDeleteStatement 的内容
    auto * delete_stmt = dynamic_cast<const dreamdb::binder::bound::BoundDeleteStatement *>(bound_delete_statement.get());
    ASSERT_NE(delete_stmt, nullptr);
    EXPECT_EQ(delete_stmt->collection_id(), 99);
    EXPECT_TRUE(delete_stmt->has_where());
    ASSERT_NE(delete_stmt->where(), nullptr);

    // 验证格式化输出
    std::string formatted = printer.format(*bound_delete_statement);
    EXPECT_TRUE(formatted.find("DELETE FROM collection_id:99") != std::string::npos);
    EXPECT_TRUE(formatted.find("WHERE") != std::string::npos);
}
