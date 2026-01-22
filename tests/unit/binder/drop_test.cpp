#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <dreamdb/binder/binder.h>
#include <dreamdb/binder/bound/debug/debug_printer.h>
#include <dreamdb/binder/bound/statement/statement.h>
#include <dreamdb/binder/bound/statement/drop.h>
#include <dreamdb/parser/ast/statement/use.h>
#include <dreamdb/parser/ast/statement/drop.h>

#include "catalog_mock.h"

using ::testing::Return;

class DropBinderTest : public ::testing::Test
{
public:
    DropBinderTest()
        : catalog_mock()
        , binder(catalog_mock)
        , printer()
    {
        // 默认行为：解析数据库与集合
        ON_CALL(catalog_mock, resolve_database("test_db"))
            .WillByDefault(Return(std::make_optional<dreamdb::common::database_id_t>(12345)));

        ON_CALL(catalog_mock, resolve_database("another_db"))
            .WillByDefault(Return(std::make_optional<dreamdb::common::database_id_t>(67890)));

        ON_CALL(catalog_mock, resolve_database("nonexistent_db"))
            .WillByDefault(Return(std::nullopt));

        ON_CALL(catalog_mock, resolve_collection(
            dreamdb::common::database_id_t{12345}, "test_collection"
        ))
            .WillByDefault(Return(std::make_optional<dreamdb::common::collection_id_t>(99)));

        ON_CALL(catalog_mock, resolve_collection(
            dreamdb::common::database_id_t{12345}, "nonexistent_collection"
        ))
            .WillByDefault(Return(std::nullopt));

        ON_CALL(catalog_mock, resolve_index(
            dreamdb::common::collection_id_t{99}, "test_index"
        ))
            .WillByDefault(Return(std::make_optional<dreamdb::common::index_id_t>(200)));

        ON_CALL(catalog_mock, resolve_index(
            dreamdb::common::collection_id_t{99}, "nonexistent_index"
        ))
            .WillByDefault(Return(std::nullopt));

        ON_CALL(catalog_mock, resolve_vindex(
            dreamdb::common::collection_id_t{99}, "test_vindex"
        ))
            .WillByDefault(Return(std::make_optional<dreamdb::common::vindex_id_t>(300)));

        ON_CALL(catalog_mock, resolve_vindex(
            dreamdb::common::collection_id_t{99}, "nonexistent_vindex"
        ))
            .WillByDefault(Return(std::nullopt));
    }

public:
    CatalogMock catalog_mock;
    dreamdb::binder::Binder binder;
    dreamdb::binder::bound::BoundDebugPrinter printer;
};

/**
 * @brief 测试绑定 DROP DATABASE 语句（存在）
 */
TEST_F(DropBinderTest, BindDropDatabase)
{
    // 创建 DROP DATABASE 语句
    dreamdb::parser::ast::AstDropDatabase drop_database;
    drop_database.database_name = "test_db";
    auto ast_drop_statement = dreamdb::parser::ast::AstDropStatement::create(
        drop_database, false, 1, 1
    );

    // 绑定语句
    auto bound_drop_statement = binder.bind(*ast_drop_statement);
    ASSERT_NE(bound_drop_statement, nullptr);

    // 验证语句类型
    EXPECT_EQ(bound_drop_statement->statement_type(), dreamdb::binder::bound::BoundStatementType::Drop);

    // 验证 BoundDropStatement 的内容
    auto * drop_stmt = dynamic_cast<const dreamdb::binder::bound::BoundDropStatement *>(bound_drop_statement.get());
    ASSERT_NE(drop_stmt, nullptr);

    const auto & operation = drop_stmt->operation();
    ASSERT_TRUE(std::holds_alternative<dreamdb::binder::bound::BoundDropDatabase>(operation));

    const auto & bound_op = std::get<dreamdb::binder::bound::BoundDropDatabase>(operation);
    EXPECT_TRUE(bound_op.database_id.has_value());
    EXPECT_EQ(bound_op.database_id.value(), 12345);

    // 验证格式化输出
    EXPECT_EQ(printer.format(*bound_drop_statement), "DROP DATABASE database_id:12345");
}

/**
 * @brief 测试绑定 DROP DATABASE 语句（不存在，不使用 IF EXISTS）
 */
TEST_F(DropBinderTest, BindDropDatabaseNonexistent)
{
    // 创建 DROP DATABASE 语句（不存在的数据库）
    dreamdb::parser::ast::AstDropDatabase drop_database;
    drop_database.database_name = "nonexistent_db";
    auto ast_drop_statement = dreamdb::parser::ast::AstDropStatement::create(
        drop_database, false, 1, 1
    );

    // 应该抛出异常
    EXPECT_THROW(
        {
            auto bound_drop_statement = binder.bind(*ast_drop_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试绑定 DROP DATABASE 语句（不存在，使用 IF EXISTS）
 */
TEST_F(DropBinderTest, BindDropDatabaseNonexistentWithIfExists)
{
    // 创建 DROP DATABASE 语句（不存在的数据库，使用 IF EXISTS）
    dreamdb::parser::ast::AstDropDatabase drop_database;
    drop_database.database_name = "nonexistent_db";
    auto ast_drop_statement = dreamdb::parser::ast::AstDropStatement::create(
        drop_database, true, 1, 1
    );

    // 绑定语句（应该成功，因为使用了 IF EXISTS）
    auto bound_drop_statement = binder.bind(*ast_drop_statement);
    ASSERT_NE(bound_drop_statement, nullptr);

    // 验证 BoundDropStatement 的内容
    auto * drop_stmt = dynamic_cast<const dreamdb::binder::bound::BoundDropStatement *>(bound_drop_statement.get());
    ASSERT_NE(drop_stmt, nullptr);

    const auto & operation = drop_stmt->operation();
    ASSERT_TRUE(std::holds_alternative<dreamdb::binder::bound::BoundDropDatabase>(operation));

    const auto & bound_op = std::get<dreamdb::binder::bound::BoundDropDatabase>(operation);
    EXPECT_FALSE(bound_op.database_id.has_value());

    // 验证格式化输出
    EXPECT_EQ(printer.format(*bound_drop_statement), "DROP DATABASE database_id:NULL");
}

/**
 * @brief 测试绑定 DROP COLLECTION 语句（存在）
 */
TEST_F(DropBinderTest, BindDropCollection)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 DROP COLLECTION 语句
    dreamdb::parser::ast::AstDropCollection drop_collection;
    drop_collection.collection_name = "test_collection";
    auto ast_drop_statement = dreamdb::parser::ast::AstDropStatement::create(
        drop_collection, false, 1, 1
    );

    // 绑定语句
    auto bound_drop_statement = binder.bind(*ast_drop_statement);
    ASSERT_NE(bound_drop_statement, nullptr);

    // 验证 BoundDropStatement 的内容
    auto * drop_stmt = dynamic_cast<const dreamdb::binder::bound::BoundDropStatement *>(bound_drop_statement.get());
    ASSERT_NE(drop_stmt, nullptr);

    const auto & operation = drop_stmt->operation();
    ASSERT_TRUE(std::holds_alternative<dreamdb::binder::bound::BoundDropCollection>(operation));

    const auto & bound_op = std::get<dreamdb::binder::bound::BoundDropCollection>(operation);
    EXPECT_TRUE(bound_op.collection_id.has_value());
    EXPECT_EQ(bound_op.collection_id.value(), 99);

    // 验证格式化输出
    EXPECT_EQ(printer.format(*bound_drop_statement), "DROP COLLECTION collection_id:99");
}

/**
 * @brief 测试绑定 DROP COLLECTION 语句（不存在，不使用 IF EXISTS）
 */
TEST_F(DropBinderTest, BindDropCollectionNonexistent)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 DROP COLLECTION 语句（不存在的集合）
    dreamdb::parser::ast::AstDropCollection drop_collection;
    drop_collection.collection_name = "nonexistent_collection";
    auto ast_drop_statement = dreamdb::parser::ast::AstDropStatement::create(
        drop_collection, false, 1, 1
    );

    // 应该抛出异常
    EXPECT_THROW(
        {
            auto bound_drop_statement = binder.bind(*ast_drop_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试绑定 DROP COLLECTION 语句（不存在，使用 IF EXISTS）
 */
TEST_F(DropBinderTest, BindDropCollectionNonexistentWithIfExists)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 DROP COLLECTION 语句（不存在的集合，使用 IF EXISTS）
    dreamdb::parser::ast::AstDropCollection drop_collection;
    drop_collection.collection_name = "nonexistent_collection";
    auto ast_drop_statement = dreamdb::parser::ast::AstDropStatement::create(
        drop_collection, true, 1, 1
    );

    // 绑定语句（应该成功，因为使用了 IF EXISTS）
    auto bound_drop_statement = binder.bind(*ast_drop_statement);
    ASSERT_NE(bound_drop_statement, nullptr);

    // 验证 BoundDropStatement 的内容
    auto * drop_stmt = dynamic_cast<const dreamdb::binder::bound::BoundDropStatement *>(bound_drop_statement.get());
    ASSERT_NE(drop_stmt, nullptr);

    const auto & operation = drop_stmt->operation();
    ASSERT_TRUE(std::holds_alternative<dreamdb::binder::bound::BoundDropCollection>(operation));

    const auto & bound_op = std::get<dreamdb::binder::bound::BoundDropCollection>(operation);
    EXPECT_FALSE(bound_op.collection_id.has_value());

    // 验证格式化输出
    EXPECT_EQ(printer.format(*bound_drop_statement), "DROP COLLECTION collection_id:NULL");
}

/**
 * @brief 测试绑定 DROP COLLECTION 语句（没有当前数据库）
 */
TEST_F(DropBinderTest, BindDropCollectionWithoutCurrentDatabase)
{
    // 不绑定 USE，直接尝试绑定 DROP COLLECTION，应该抛出异常
    dreamdb::parser::ast::AstDropCollection drop_collection;
    drop_collection.collection_name = "test_collection";
    auto ast_drop_statement = dreamdb::parser::ast::AstDropStatement::create(
        drop_collection, false, 1, 1
    );

    EXPECT_THROW(
        {
            auto bound_drop_statement = binder.bind(*ast_drop_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试绑定 DROP INDEX 语句（存在）
 */
TEST_F(DropBinderTest, BindDropIndex)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 DROP INDEX 语句
    dreamdb::parser::ast::AstDropIndex drop_index;
    drop_index.index_name = "test_index";
    drop_index.collection_name = "test_collection";
    auto ast_drop_statement = dreamdb::parser::ast::AstDropStatement::create(
        drop_index, false, 1, 1
    );

    // 绑定语句
    auto bound_drop_statement = binder.bind(*ast_drop_statement);
    ASSERT_NE(bound_drop_statement, nullptr);

    // 验证 BoundDropStatement 的内容
    auto * drop_stmt = dynamic_cast<const dreamdb::binder::bound::BoundDropStatement *>(bound_drop_statement.get());
    ASSERT_NE(drop_stmt, nullptr);

    const auto & operation = drop_stmt->operation();
    ASSERT_TRUE(std::holds_alternative<dreamdb::binder::bound::BoundDropIndex>(operation));

    const auto & bound_op = std::get<dreamdb::binder::bound::BoundDropIndex>(operation);
    EXPECT_TRUE(bound_op.index_id.has_value());
    EXPECT_EQ(bound_op.index_id.value(), 200);

    // 验证格式化输出
    EXPECT_EQ(printer.format(*bound_drop_statement), "DROP INDEX index_id:200");
}

/**
 * @brief 测试绑定 DROP INDEX 语句（不存在，不使用 IF EXISTS）
 */
TEST_F(DropBinderTest, BindDropIndexNonexistent)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 DROP INDEX 语句（不存在的索引）
    dreamdb::parser::ast::AstDropIndex drop_index;
    drop_index.index_name = "nonexistent_index";
    drop_index.collection_name = "test_collection";
    auto ast_drop_statement = dreamdb::parser::ast::AstDropStatement::create(
        drop_index, false, 1, 1
    );

    // 应该抛出异常
    EXPECT_THROW(
        {
            auto bound_drop_statement = binder.bind(*ast_drop_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试绑定 DROP INDEX 语句（不存在，使用 IF EXISTS）
 */
TEST_F(DropBinderTest, BindDropIndexNonexistentWithIfExists)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 DROP INDEX 语句（不存在的索引，使用 IF EXISTS）
    dreamdb::parser::ast::AstDropIndex drop_index;
    drop_index.index_name = "nonexistent_index";
    drop_index.collection_name = "test_collection";
    auto ast_drop_statement = dreamdb::parser::ast::AstDropStatement::create(
        drop_index, true, 1, 1
    );

    // 绑定语句（应该成功，因为使用了 IF EXISTS）
    auto bound_drop_statement = binder.bind(*ast_drop_statement);
    ASSERT_NE(bound_drop_statement, nullptr);

    // 验证 BoundDropStatement 的内容
    auto * drop_stmt = dynamic_cast<const dreamdb::binder::bound::BoundDropStatement *>(bound_drop_statement.get());
    ASSERT_NE(drop_stmt, nullptr);

    const auto & operation = drop_stmt->operation();
    ASSERT_TRUE(std::holds_alternative<dreamdb::binder::bound::BoundDropIndex>(operation));

    const auto & bound_op = std::get<dreamdb::binder::bound::BoundDropIndex>(operation);
    EXPECT_FALSE(bound_op.index_id.has_value());

    // 验证格式化输出
    EXPECT_EQ(printer.format(*bound_drop_statement), "DROP INDEX index_id:NULL");
}

/**
 * @brief 测试绑定 DROP INDEX 语句（没有当前数据库）
 */
TEST_F(DropBinderTest, BindDropIndexWithoutCurrentDatabase)
{
    // 不绑定 USE，直接尝试绑定 DROP INDEX，应该抛出异常
    dreamdb::parser::ast::AstDropIndex drop_index;
    drop_index.index_name = "test_index";
    drop_index.collection_name = "test_collection";
    auto ast_drop_statement = dreamdb::parser::ast::AstDropStatement::create(
        drop_index, false, 1, 1
    );

    EXPECT_THROW(
        {
            auto bound_drop_statement = binder.bind(*ast_drop_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试绑定 DROP VINDEX 语句（存在）
 */
TEST_F(DropBinderTest, BindDropVIndex)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 DROP VINDEX 语句
    dreamdb::parser::ast::AstDropVIndex drop_vindex;
    drop_vindex.vindex_name = "test_vindex";
    drop_vindex.collection_name = "test_collection";
    auto ast_drop_statement = dreamdb::parser::ast::AstDropStatement::create(
        drop_vindex, false, 1, 1
    );

    // 绑定语句
    auto bound_drop_statement = binder.bind(*ast_drop_statement);
    ASSERT_NE(bound_drop_statement, nullptr);

    // 验证 BoundDropStatement 的内容
    auto * drop_stmt = dynamic_cast<const dreamdb::binder::bound::BoundDropStatement *>(bound_drop_statement.get());
    ASSERT_NE(drop_stmt, nullptr);

    const auto & operation = drop_stmt->operation();
    ASSERT_TRUE(std::holds_alternative<dreamdb::binder::bound::BoundDropVIndex>(operation));

    const auto & bound_op = std::get<dreamdb::binder::bound::BoundDropVIndex>(operation);
    EXPECT_TRUE(bound_op.vindex_id.has_value());
    EXPECT_EQ(bound_op.vindex_id.value(), 300);

    // 验证格式化输出
    EXPECT_EQ(printer.format(*bound_drop_statement), "DROP VINDEX vindex_id:300");
}

/**
 * @brief 测试绑定 DROP VINDEX 语句（不存在，不使用 IF EXISTS）
 */
TEST_F(DropBinderTest, BindDropVIndexNonexistent)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 DROP VINDEX 语句（不存在的向量索引）
    dreamdb::parser::ast::AstDropVIndex drop_vindex;
    drop_vindex.vindex_name = "nonexistent_vindex";
    drop_vindex.collection_name = "test_collection";
    auto ast_drop_statement = dreamdb::parser::ast::AstDropStatement::create(
        drop_vindex, false, 1, 1
    );

    // 应该抛出异常
    EXPECT_THROW(
        {
            auto bound_drop_statement = binder.bind(*ast_drop_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试绑定 DROP VINDEX 语句（不存在，使用 IF EXISTS）
 */
TEST_F(DropBinderTest, BindDropVIndexNonexistentWithIfExists)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 DROP VINDEX 语句（不存在的向量索引，使用 IF EXISTS）
    dreamdb::parser::ast::AstDropVIndex drop_vindex;
    drop_vindex.vindex_name = "nonexistent_vindex";
    drop_vindex.collection_name = "test_collection";
    auto ast_drop_statement = dreamdb::parser::ast::AstDropStatement::create(
        drop_vindex, true, 1, 1
    );

    // 绑定语句（应该成功，因为使用了 IF EXISTS）
    auto bound_drop_statement = binder.bind(*ast_drop_statement);
    ASSERT_NE(bound_drop_statement, nullptr);

    // 验证 BoundDropStatement 的内容
    auto * drop_stmt = dynamic_cast<const dreamdb::binder::bound::BoundDropStatement *>(bound_drop_statement.get());
    ASSERT_NE(drop_stmt, nullptr);

    const auto & operation = drop_stmt->operation();
    ASSERT_TRUE(std::holds_alternative<dreamdb::binder::bound::BoundDropVIndex>(operation));

    const auto & bound_op = std::get<dreamdb::binder::bound::BoundDropVIndex>(operation);
    EXPECT_FALSE(bound_op.vindex_id.has_value());

    // 验证格式化输出
    EXPECT_EQ(printer.format(*bound_drop_statement), "DROP VINDEX vindex_id:NULL");
}

/**
 * @brief 测试绑定 DROP VINDEX 语句（没有当前数据库）
 */
TEST_F(DropBinderTest, BindDropVIndexWithoutCurrentDatabase)
{
    // 不绑定 USE，直接尝试绑定 DROP VINDEX，应该抛出异常
    dreamdb::parser::ast::AstDropVIndex drop_vindex;
    drop_vindex.vindex_name = "test_vindex";
    drop_vindex.collection_name = "test_collection";
    auto ast_drop_statement = dreamdb::parser::ast::AstDropStatement::create(
        drop_vindex, false, 1, 1
    );

    EXPECT_THROW(
        {
            auto bound_drop_statement = binder.bind(*ast_drop_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试 DROP 语句的类型
 */
TEST_F(DropBinderTest, DropStatementType)
{
    // 创建 DROP DATABASE 语句
    dreamdb::parser::ast::AstDropDatabase drop_database;
    drop_database.database_name = "test_db";
    auto ast_drop_statement = dreamdb::parser::ast::AstDropStatement::create(
        drop_database, false, 1, 1
    );

    // 绑定语句
    auto bound_drop_statement = binder.bind(*ast_drop_statement);
    ASSERT_NE(bound_drop_statement, nullptr);

    // 验证语句类型
    EXPECT_EQ(bound_drop_statement->statement_type(), dreamdb::binder::bound::BoundStatementType::Drop);
}

