#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <dreamdb/binder/binder.h>
#include <dreamdb/binder/bound/debug/debug_printer.h>
#include <dreamdb/binder/bound/statement/statement.h>
#include <dreamdb/binder/bound/statement/describe.h>
#include <dreamdb/parser/ast/statement/use.h>
#include <dreamdb/parser/ast/statement/describe.h>

#include "catalog_mock.h"

using ::testing::Return;

class DescribeBinderTest : public ::testing::Test
{
public:
    DescribeBinderTest()
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
    }

public:
    CatalogMock catalog_mock;
    dreamdb::binder::Binder binder;
    dreamdb::binder::bound::BoundDebugPrinter printer;
};

/**
 * @brief 测试正常绑定 DESCRIBE 语句
 */
TEST_F(DescribeBinderTest, BindDescribeStatement)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 再绑定 DESCRIBE
    auto ast_describe_statement = dreamdb::parser::ast::AstDescribeStatement::create("test_collection", 1, 1);
    auto bound_describe_statement = binder.bind(*ast_describe_statement);
    ASSERT_NE(bound_describe_statement, nullptr);

    // 验证绑定结果
    EXPECT_EQ(printer.format(*bound_describe_statement), "DESCRIBE collection_id:99");

    // 验证 BoundDescribeStatement 的内容
    auto * describe_stmt = dynamic_cast<const dreamdb::binder::bound::BoundDescribeStatement *>(bound_describe_statement.get());
    ASSERT_NE(describe_stmt, nullptr);
    EXPECT_EQ(describe_stmt->collection_id(), 99);
}

/**
 * @brief 测试绑定不同的集合
 */
TEST_F(DescribeBinderTest, BindDescribeDifferentCollection)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 绑定另一个集合
    auto ast_describe_statement = dreamdb::parser::ast::AstDescribeStatement::create("another_collection", 1, 1);
    auto bound_describe_statement = binder.bind(*ast_describe_statement);
    ASSERT_NE(bound_describe_statement, nullptr);

    // 验证绑定结果
    EXPECT_EQ(printer.format(*bound_describe_statement), "DESCRIBE collection_id:100");

    // 验证 BoundDescribeStatement 的内容
    auto * describe_stmt = dynamic_cast<const dreamdb::binder::bound::BoundDescribeStatement *>(bound_describe_statement.get());
    ASSERT_NE(describe_stmt, nullptr);
    EXPECT_EQ(describe_stmt->collection_id(), 100);
}

/**
 * @brief 测试集合不存在的情况
 */
TEST_F(DescribeBinderTest, BindDescribeNonexistentCollection)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 尝试绑定不存在的集合，应该抛出异常
    auto ast_describe_statement = dreamdb::parser::ast::AstDescribeStatement::create("nonexistent_collection", 1, 1);
    
    EXPECT_THROW(
        {
            auto bound_describe_statement = binder.bind(*ast_describe_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试没有设置当前数据库的情况
 */
TEST_F(DescribeBinderTest, BindDescribeWithoutCurrentDatabase)
{
    // 不绑定 USE，直接尝试绑定 DESCRIBE，应该抛出异常
    auto ast_describe_statement = dreamdb::parser::ast::AstDescribeStatement::create("test_collection", 1, 1);
    
    EXPECT_THROW(
        {
            auto bound_describe_statement = binder.bind(*ast_describe_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试 DESCRIBE 语句的类型
 */
TEST_F(DescribeBinderTest, DescribeStatementType)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 绑定 DESCRIBE
    auto ast_describe_statement = dreamdb::parser::ast::AstDescribeStatement::create("test_collection", 1, 1);
    auto bound_describe_statement = binder.bind(*ast_describe_statement);
    ASSERT_NE(bound_describe_statement, nullptr);

    // 验证语句类型
    EXPECT_EQ(bound_describe_statement->statement_type(), dreamdb::binder::bound::BoundStatementType::Describe);
}
