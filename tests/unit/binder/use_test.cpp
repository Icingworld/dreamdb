#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <dreamdb/binder/binder.h>
#include <dreamdb/binder/bound/debug/debug_printer.h>
#include <dreamdb/binder/bound/statement/statement.h>
#include <dreamdb/binder/bound/statement/use.h>
#include <dreamdb/parser/ast/statement/use.h>

#include "catalog_mock.h"

using ::testing::Return;

class UseBinderTest : public ::testing::Test
{
public:
    UseBinderTest()
        : catalog_mock()
        , binder(catalog_mock)
        , printer()
    {
        // 模拟解析数据库信息
        ON_CALL(catalog_mock, resolve_database("test_db"))
            .WillByDefault(Return(std::make_optional<dreamdb::common::database_id_t>(12345)));

        ON_CALL(catalog_mock, resolve_database("another_db"))
            .WillByDefault(Return(std::make_optional<dreamdb::common::database_id_t>(67890)));

        ON_CALL(catalog_mock, resolve_database("nonexistent_db"))
            .WillByDefault(Return(std::nullopt));
    }

public:
    CatalogMock catalog_mock;
    dreamdb::binder::Binder binder;
    dreamdb::binder::bound::BoundDebugPrinter printer;
};

/**
 * @brief 测试正常绑定 USE 语句
 */
TEST_F(UseBinderTest, BindUseStatement)
{
    // 构造 USE 语句语法树
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create(
        "test_db", 1, 1
    );

    // 绑定语句
    auto bound_statement = binder.bind(*ast_use_statement);

    ASSERT_NE(bound_statement, nullptr);

    // 验证格式化输出
    EXPECT_EQ(printer.format(*bound_statement), "USE database_id:12345");

    // 验证 BoundUseStatement 的内容
    auto * use_stmt = dynamic_cast<const dreamdb::binder::bound::BoundUseStatement *>(bound_statement.get());
    ASSERT_NE(use_stmt, nullptr);
    EXPECT_EQ(use_stmt->database_id(), 12345);
}

/**
 * @brief 测试绑定不同的数据库
 */
TEST_F(UseBinderTest, BindUseDifferentDatabase)
{
    // 构造 USE 语句语法树（不同的数据库）
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create(
        "another_db", 1, 1
    );

    // 绑定语句
    auto bound_statement = binder.bind(*ast_use_statement);

    ASSERT_NE(bound_statement, nullptr);

    // 验证格式化输出
    EXPECT_EQ(printer.format(*bound_statement), "USE database_id:67890");

    // 验证 BoundUseStatement 的内容
    auto * use_stmt = dynamic_cast<const dreamdb::binder::bound::BoundUseStatement *>(bound_statement.get());
    ASSERT_NE(use_stmt, nullptr);
    EXPECT_EQ(use_stmt->database_id(), 67890);
}

/**
 * @brief 测试数据库不存在的情况
 */
TEST_F(UseBinderTest, BindUseNonexistentDatabase)
{
    // 构造 USE 语句语法树（不存在的数据库）
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create(
        "nonexistent_db", 1, 1
    );

    // 应该抛出异常
    EXPECT_THROW(
        {
            auto bound_statement = binder.bind(*ast_use_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试 USE 语句的类型
 */
TEST_F(UseBinderTest, UseStatementType)
{
    // 构造 USE 语句语法树
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create(
        "test_db", 1, 1
    );

    // 绑定语句
    auto bound_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_statement, nullptr);

    // 验证语句类型
    EXPECT_EQ(bound_statement->statement_type(), dreamdb::binder::bound::BoundStatementType::Use);
}

/**
 * @brief 测试连续使用 USE 切换数据库
 */
TEST_F(UseBinderTest, BindMultipleUseStatements)
{
    // 第一次 USE
    auto ast_use_statement1 = dreamdb::parser::ast::AstUseStatement::create(
        "test_db", 1, 1
    );
    auto bound_statement1 = binder.bind(*ast_use_statement1);
    ASSERT_NE(bound_statement1, nullptr);

    auto * use_stmt1 = dynamic_cast<const dreamdb::binder::bound::BoundUseStatement *>(bound_statement1.get());
    ASSERT_NE(use_stmt1, nullptr);
    EXPECT_EQ(use_stmt1->database_id(), 12345);

    // 第二次 USE（切换到另一个数据库）
    auto ast_use_statement2 = dreamdb::parser::ast::AstUseStatement::create(
        "another_db", 1, 1
    );
    auto bound_statement2 = binder.bind(*ast_use_statement2);
    ASSERT_NE(bound_statement2, nullptr);

    auto * use_stmt2 = dynamic_cast<const dreamdb::binder::bound::BoundUseStatement *>(bound_statement2.get());
    ASSERT_NE(use_stmt2, nullptr);
    EXPECT_EQ(use_stmt2->database_id(), 67890);
}
