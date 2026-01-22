#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <dreamdb/binder/binder.h>
#include <dreamdb/binder/bound/debug/debug_printer.h>
#include <dreamdb/binder/bound/statement/statement.h>
#include <dreamdb/binder/bound/statement/show.h>
#include <dreamdb/parser/ast/statement/use.h>
#include <dreamdb/parser/ast/statement/show.h>

#include "catalog_mock.h"

using ::testing::Return;

class ShowBinderTest : public ::testing::Test
{
public:
    ShowBinderTest()
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
            dreamdb::common::database_id_t{67890}, "test_collection"
        ))
            .WillByDefault(Return(std::make_optional<dreamdb::common::collection_id_t>(100)));

        ON_CALL(catalog_mock, resolve_collection(
            dreamdb::common::database_id_t{12345}, "nonexistent_collection"
        ))
            .WillByDefault(Return(std::nullopt));
    }

public:
    CatalogMock catalog_mock;
    dreamdb::binder::Binder binder;
    dreamdb::binder::bound::BoundDebugPrinter printer;
};

/**
 * @brief 测试绑定 SHOW DATABASES 语句
 */
TEST_F(ShowBinderTest, BindShowDatabases)
{
    // 创建 SHOW DATABASES 语句
    dreamdb::parser::ast::AstShowDatabases show_databases;
    auto ast_show_statement = dreamdb::parser::ast::AstShowStatement::create(
        show_databases, 1, 1
    );

    // 绑定语句
    auto bound_show_statement = binder.bind(*ast_show_statement);
    ASSERT_NE(bound_show_statement, nullptr);

    // 验证语句类型
    EXPECT_EQ(bound_show_statement->statement_type(), dreamdb::binder::bound::BoundStatementType::Show);

    // 验证 BoundShowStatement 的内容
    auto * show_stmt = dynamic_cast<const dreamdb::binder::bound::BoundShowStatement *>(bound_show_statement.get());
    ASSERT_NE(show_stmt, nullptr);

    // 验证操作类型
    const auto & operation = show_stmt->operation();
    EXPECT_TRUE(std::holds_alternative<dreamdb::binder::bound::BoundShowDatabases>(operation));

    // 验证格式化输出
    EXPECT_EQ(printer.format(*bound_show_statement), "SHOW DATABASES");
}

/**
 * @brief 测试绑定 SHOW COLLECTIONS 语句（不指定数据库，使用当前数据库）
 */
TEST_F(ShowBinderTest, BindShowCollectionsWithoutDatabase)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 SHOW COLLECTIONS 语句（不指定数据库）
    dreamdb::parser::ast::AstShowCollections show_collections;
    show_collections.database_name = std::nullopt;
    auto ast_show_statement = dreamdb::parser::ast::AstShowStatement::create(
        show_collections, 1, 1
    );

    // 绑定语句
    auto bound_show_statement = binder.bind(*ast_show_statement);
    ASSERT_NE(bound_show_statement, nullptr);

    // 验证 BoundShowStatement 的内容
    auto * show_stmt = dynamic_cast<const dreamdb::binder::bound::BoundShowStatement *>(bound_show_statement.get());
    ASSERT_NE(show_stmt, nullptr);

    const auto & operation = show_stmt->operation();
    ASSERT_TRUE(std::holds_alternative<dreamdb::binder::bound::BoundShowCollections>(operation));
    
    const auto & bound_op = std::get<dreamdb::binder::bound::BoundShowCollections>(operation);
    // 应该设置为 nullopt，让 Executor 使用当前数据库
    EXPECT_FALSE(bound_op.database_id.has_value());
    
    // 验证格式化输出
    EXPECT_EQ(printer.format(*bound_show_statement), "SHOW COLLECTIONS");
}

/**
 * @brief 测试绑定 SHOW COLLECTIONS 语句（指定数据库）
 */
TEST_F(ShowBinderTest, BindShowCollectionsWithDatabase)
{
    // 创建 SHOW COLLECTIONS 语句（指定数据库）
    dreamdb::parser::ast::AstShowCollections show_collections;
    show_collections.database_name = "test_db";
    auto ast_show_statement = dreamdb::parser::ast::AstShowStatement::create(
        show_collections, 1, 1
    );

    // 绑定语句
    auto bound_show_statement = binder.bind(*ast_show_statement);
    ASSERT_NE(bound_show_statement, nullptr);

    // 验证 BoundShowStatement 的内容
    auto * show_stmt = dynamic_cast<const dreamdb::binder::bound::BoundShowStatement *>(bound_show_statement.get());
    ASSERT_NE(show_stmt, nullptr);

    const auto & operation = show_stmt->operation();
    ASSERT_TRUE(std::holds_alternative<dreamdb::binder::bound::BoundShowCollections>(operation));
    
    const auto & bound_op = std::get<dreamdb::binder::bound::BoundShowCollections>(operation);
    EXPECT_TRUE(bound_op.database_id.has_value());
    EXPECT_EQ(bound_op.database_id.value(), 12345);
    
    // 验证格式化输出
    EXPECT_EQ(printer.format(*bound_show_statement), "SHOW COLLECTIONS FROM database_id:12345");
}

/**
 * @brief 测试绑定 SHOW COLLECTIONS 语句（数据库不存在）
 */
TEST_F(ShowBinderTest, BindShowCollectionsNonexistentDatabase)
{
    // 创建 SHOW COLLECTIONS 语句（指定不存在的数据库）
    dreamdb::parser::ast::AstShowCollections show_collections;
    show_collections.database_name = "nonexistent_db";
    auto ast_show_statement = dreamdb::parser::ast::AstShowStatement::create(
        show_collections, 1, 1
    );

    // 应该抛出异常
    EXPECT_THROW(
        {
            auto bound_show_statement = binder.bind(*ast_show_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试绑定 SHOW INDEXES 语句（不指定数据库，使用当前数据库）
 */
TEST_F(ShowBinderTest, BindShowIndexesWithoutDatabase)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 SHOW INDEXES 语句（不指定数据库）
    dreamdb::parser::ast::AstShowIndexes show_indexes;
    show_indexes.collection_name = "test_collection";
    show_indexes.database_name = std::nullopt;
    auto ast_show_statement = dreamdb::parser::ast::AstShowStatement::create(
        show_indexes, 1, 1
    );

    // 绑定语句
    auto bound_show_statement = binder.bind(*ast_show_statement);
    ASSERT_NE(bound_show_statement, nullptr);

    // 验证 BoundShowStatement 的内容
    auto * show_stmt = dynamic_cast<const dreamdb::binder::bound::BoundShowStatement *>(bound_show_statement.get());
    ASSERT_NE(show_stmt, nullptr);

    const auto & operation = show_stmt->operation();
    ASSERT_TRUE(std::holds_alternative<dreamdb::binder::bound::BoundShowIndexes>(operation));
    
    const auto & bound_op = std::get<dreamdb::binder::bound::BoundShowIndexes>(operation);
    EXPECT_EQ(bound_op.collection_id, 99);
    
    // 验证格式化输出
    EXPECT_EQ(printer.format(*bound_show_statement), "SHOW INDEXES FROM collection_id:99");
}

/**
 * @brief 测试绑定 SHOW INDEXES 语句（指定数据库）
 */
TEST_F(ShowBinderTest, BindShowIndexesWithDatabase)
{
    // 创建 SHOW INDEXES 语句（指定数据库）
    dreamdb::parser::ast::AstShowIndexes show_indexes;
    show_indexes.collection_name = "test_collection";
    show_indexes.database_name = "another_db";
    auto ast_show_statement = dreamdb::parser::ast::AstShowStatement::create(
        show_indexes, 1, 1
    );

    // 绑定语句
    auto bound_show_statement = binder.bind(*ast_show_statement);
    ASSERT_NE(bound_show_statement, nullptr);

    // 验证 BoundShowStatement 的内容
    auto * show_stmt = dynamic_cast<const dreamdb::binder::bound::BoundShowStatement *>(bound_show_statement.get());
    ASSERT_NE(show_stmt, nullptr);

    const auto & operation = show_stmt->operation();
    ASSERT_TRUE(std::holds_alternative<dreamdb::binder::bound::BoundShowIndexes>(operation));
    
    const auto & bound_op = std::get<dreamdb::binder::bound::BoundShowIndexes>(operation);
    EXPECT_EQ(bound_op.collection_id, 100);
    
    // 验证格式化输出
    EXPECT_EQ(printer.format(*bound_show_statement), "SHOW INDEXES FROM collection_id:100");
}

/**
 * @brief 测试绑定 SHOW INDEXES 语句（集合不存在）
 */
TEST_F(ShowBinderTest, BindShowIndexesNonexistentCollection)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 SHOW INDEXES 语句（集合不存在）
    dreamdb::parser::ast::AstShowIndexes show_indexes;
    show_indexes.collection_name = "nonexistent_collection";
    show_indexes.database_name = std::nullopt;
    auto ast_show_statement = dreamdb::parser::ast::AstShowStatement::create(
        show_indexes, 1, 1
    );

    // 应该抛出异常
    EXPECT_THROW(
        {
            auto bound_show_statement = binder.bind(*ast_show_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试绑定 SHOW INDEXES 语句（数据库不存在）
 */
TEST_F(ShowBinderTest, BindShowIndexesNonexistentDatabase)
{
    // 创建 SHOW INDEXES 语句（数据库不存在）
    dreamdb::parser::ast::AstShowIndexes show_indexes;
    show_indexes.collection_name = "test_collection";
    show_indexes.database_name = "nonexistent_db";
    auto ast_show_statement = dreamdb::parser::ast::AstShowStatement::create(
        show_indexes, 1, 1
    );

    // 应该抛出异常
    EXPECT_THROW(
        {
            auto bound_show_statement = binder.bind(*ast_show_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试绑定 SHOW INDEXES 语句（没有当前数据库）
 */
TEST_F(ShowBinderTest, BindShowIndexesWithoutCurrentDatabase)
{
    // 不绑定 USE，直接尝试绑定 SHOW INDEXES（不指定数据库）
    dreamdb::parser::ast::AstShowIndexes show_indexes;
    show_indexes.collection_name = "test_collection";
    show_indexes.database_name = std::nullopt;
    auto ast_show_statement = dreamdb::parser::ast::AstShowStatement::create(
        show_indexes, 1, 1
    );

    // 应该抛出异常
    EXPECT_THROW(
        {
            auto bound_show_statement = binder.bind(*ast_show_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试绑定 SHOW VINDEXES 语句（不指定数据库，使用当前数据库）
 */
TEST_F(ShowBinderTest, BindShowVIndexesWithoutDatabase)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 SHOW VINDEXES 语句（不指定数据库）
    dreamdb::parser::ast::AstShowVIndexes show_vindexes;
    show_vindexes.collection_name = "test_collection";
    show_vindexes.database_name = std::nullopt;
    auto ast_show_statement = dreamdb::parser::ast::AstShowStatement::create(
        show_vindexes, 1, 1
    );

    // 绑定语句
    auto bound_show_statement = binder.bind(*ast_show_statement);
    ASSERT_NE(bound_show_statement, nullptr);

    // 验证 BoundShowStatement 的内容
    auto * show_stmt = dynamic_cast<const dreamdb::binder::bound::BoundShowStatement *>(bound_show_statement.get());
    ASSERT_NE(show_stmt, nullptr);

    const auto & operation = show_stmt->operation();
    ASSERT_TRUE(std::holds_alternative<dreamdb::binder::bound::BoundShowVIndexes>(operation));
    
    const auto & bound_op = std::get<dreamdb::binder::bound::BoundShowVIndexes>(operation);
    EXPECT_EQ(bound_op.collection_id, 99);
    
    // 验证格式化输出
    EXPECT_EQ(printer.format(*bound_show_statement), "SHOW VINDEXES FROM collection_id:99");
}

/**
 * @brief 测试绑定 SHOW VINDEXES 语句（指定数据库）
 */
TEST_F(ShowBinderTest, BindShowVIndexesWithDatabase)
{
    // 创建 SHOW VINDEXES 语句（指定数据库）
    dreamdb::parser::ast::AstShowVIndexes show_vindexes;
    show_vindexes.collection_name = "test_collection";
    show_vindexes.database_name = "another_db";
    auto ast_show_statement = dreamdb::parser::ast::AstShowStatement::create(
        show_vindexes, 1, 1
    );

    // 绑定语句
    auto bound_show_statement = binder.bind(*ast_show_statement);
    ASSERT_NE(bound_show_statement, nullptr);

    // 验证 BoundShowStatement 的内容
    auto * show_stmt = dynamic_cast<const dreamdb::binder::bound::BoundShowStatement *>(bound_show_statement.get());
    ASSERT_NE(show_stmt, nullptr);

    const auto & operation = show_stmt->operation();
    ASSERT_TRUE(std::holds_alternative<dreamdb::binder::bound::BoundShowVIndexes>(operation));
    
    const auto & bound_op = std::get<dreamdb::binder::bound::BoundShowVIndexes>(operation);
    EXPECT_EQ(bound_op.collection_id, 100);
    
    // 验证格式化输出
    EXPECT_EQ(printer.format(*bound_show_statement), "SHOW VINDEXES FROM collection_id:100");
}

/**
 * @brief 测试绑定 SHOW VINDEXES 语句（集合不存在）
 */
TEST_F(ShowBinderTest, BindShowVIndexesNonexistentCollection)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 SHOW VINDEXES 语句（集合不存在）
    dreamdb::parser::ast::AstShowVIndexes show_vindexes;
    show_vindexes.collection_name = "nonexistent_collection";
    show_vindexes.database_name = std::nullopt;
    auto ast_show_statement = dreamdb::parser::ast::AstShowStatement::create(
        show_vindexes, 1, 1
    );

    // 应该抛出异常
    EXPECT_THROW(
        {
            auto bound_show_statement = binder.bind(*ast_show_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试绑定 SHOW VINDEXES 语句（数据库不存在）
 */
TEST_F(ShowBinderTest, BindShowVIndexesNonexistentDatabase)
{
    // 创建 SHOW VINDEXES 语句（数据库不存在）
    dreamdb::parser::ast::AstShowVIndexes show_vindexes;
    show_vindexes.collection_name = "test_collection";
    show_vindexes.database_name = "nonexistent_db";
    auto ast_show_statement = dreamdb::parser::ast::AstShowStatement::create(
        show_vindexes, 1, 1
    );

    // 应该抛出异常
    EXPECT_THROW(
        {
            auto bound_show_statement = binder.bind(*ast_show_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试绑定 SHOW VINDEXES 语句（没有当前数据库）
 */
TEST_F(ShowBinderTest, BindShowVIndexesWithoutCurrentDatabase)
{
    // 不绑定 USE，直接尝试绑定 SHOW VINDEXES（不指定数据库）
    dreamdb::parser::ast::AstShowVIndexes show_vindexes;
    show_vindexes.collection_name = "test_collection";
    show_vindexes.database_name = std::nullopt;
    auto ast_show_statement = dreamdb::parser::ast::AstShowStatement::create(
        show_vindexes, 1, 1
    );

    // 应该抛出异常
    EXPECT_THROW(
        {
            auto bound_show_statement = binder.bind(*ast_show_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试 SHOW 语句的类型
 */
TEST_F(ShowBinderTest, ShowStatementType)
{
    // 创建 SHOW DATABASES 语句
    dreamdb::parser::ast::AstShowDatabases show_databases;
    auto ast_show_statement = dreamdb::parser::ast::AstShowStatement::create(
        show_databases, 1, 1
    );

    // 绑定语句
    auto bound_show_statement = binder.bind(*ast_show_statement);
    ASSERT_NE(bound_show_statement, nullptr);

    // 验证语句类型
    EXPECT_EQ(bound_show_statement->statement_type(), dreamdb::binder::bound::BoundStatementType::Show);
}
