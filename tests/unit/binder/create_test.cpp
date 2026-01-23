#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <dreamdb/binder/binder.h>
#include <dreamdb/binder/bound/statement/statement.h>
#include <dreamdb/binder/bound/statement/create.h>
#include <dreamdb/parser/ast/statement/use.h>
#include <dreamdb/parser/ast/statement/create.h>
#include <dreamdb/parser/ast/statement/column_definition.h>
#include <dreamdb/parser/ast/expression/literal.h>

#include "catalog_mock.h"

using ::testing::Return;

class CreateBinderTest : public ::testing::Test
{
public:
    CreateBinderTest()
        : catalog_mock()
        , binder(catalog_mock)
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

        // 设置列解析（用于 CREATE INDEX / VINDEX）
        dreamdb::catalog::column_info_t id_column_info{
            dreamdb::common::column_id_t{0},
            0,
            "id",
            dreamdb::common::LogicalType{dreamdb::common::LogicalTypeId::Integer},
            false,
            true
        };
        ON_CALL(catalog_mock, resolve_column(dreamdb::common::collection_id_t{99}, "id"))
            .WillByDefault(Return(std::make_optional<dreamdb::catalog::column_info_t>(id_column_info)));

        dreamdb::catalog::column_info_t vec_column_info{
            dreamdb::common::column_id_t{10},
            10,
            "embedding",
            dreamdb::common::LogicalType{dreamdb::common::LogicalTypeId::Vector},
            false,
            true
        };
        ON_CALL(catalog_mock, resolve_column(dreamdb::common::collection_id_t{99}, "embedding"))
            .WillByDefault(Return(std::make_optional<dreamdb::catalog::column_info_t>(vec_column_info)));

        ON_CALL(catalog_mock, resolve_column(dreamdb::common::collection_id_t{99}, "nonexistent_column"))
            .WillByDefault(Return(std::nullopt));
    }

public:
    CatalogMock catalog_mock;
    dreamdb::binder::Binder binder;
};

TEST_F(CreateBinderTest, BindCreateDatabase)
{
    dreamdb::parser::ast::AstCreateDatabase op{"db1"};
    auto ast = dreamdb::parser::ast::AstCreateStatement::create(std::move(op), false, 1, 1);

    auto bound = binder.bind(*ast);
    ASSERT_NE(bound, nullptr);
    EXPECT_EQ(bound->statement_type(), dreamdb::binder::bound::BoundStatementType::Create);

    auto * create_stmt = dynamic_cast<const dreamdb::binder::bound::BoundCreateStatement *>(bound.get());
    ASSERT_NE(create_stmt, nullptr);
    EXPECT_FALSE(create_stmt->if_not_exists());
    ASSERT_TRUE(std::holds_alternative<dreamdb::binder::bound::BoundCreateDatabase>(create_stmt->create_operation()));
    EXPECT_EQ(std::get<dreamdb::binder::bound::BoundCreateDatabase>(create_stmt->create_operation()).database_name, "db1");
}

TEST_F(CreateBinderTest, BindCreateDatabaseIfNotExists)
{
    dreamdb::parser::ast::AstCreateDatabase op{"db1"};
    auto ast = dreamdb::parser::ast::AstCreateStatement::create(std::move(op), true, 1, 1);

    auto bound = binder.bind(*ast);
    ASSERT_NE(bound, nullptr);

    auto * create_stmt = dynamic_cast<const dreamdb::binder::bound::BoundCreateStatement *>(bound.get());
    ASSERT_NE(create_stmt, nullptr);
    EXPECT_TRUE(create_stmt->if_not_exists());
}

TEST_F(CreateBinderTest, BindCreateCollectionWithColumns)
{
    // CREATE COLLECTION test_collection (id INTEGER NOT NULL DEFAULT 1, name VARCHAR(10))
    std::vector<dreamdb::parser::ast::AstColumnDefinition> defs;

    {
        std::vector<std::unique_ptr<dreamdb::parser::ast::AstExpression>> args;
        std::vector<dreamdb::parser::ast::AstColumnModifier> modifiers{dreamdb::parser::ast::AstColumnModifier::NotNull};
        auto default_value = dreamdb::parser::ast::AstLiteralExpression::create_integer(1, 1, 1);
        defs.emplace_back(
            "id",
            "INTEGER",
            std::move(args),
            std::move(modifiers),
            std::move(default_value),
            std::nullopt
        );
    }

    {
        std::vector<std::unique_ptr<dreamdb::parser::ast::AstExpression>> args;
        args.push_back(dreamdb::parser::ast::AstLiteralExpression::create_integer(10, 1, 1));
        std::vector<dreamdb::parser::ast::AstColumnModifier> modifiers;
        defs.emplace_back(
            "name",
            "VARCHAR",
            std::move(args),
            std::move(modifiers),
            nullptr,
            std::nullopt
        );
    }

    dreamdb::parser::ast::AstCreateCollection op;
    op.collection_name = "test_collection";
    op.column_definitions = std::move(defs);

    auto ast = dreamdb::parser::ast::AstCreateStatement::create(std::move(op), false, 1, 1);
    auto bound = binder.bind(*ast);
    ASSERT_NE(bound, nullptr);

    auto * create_stmt = dynamic_cast<const dreamdb::binder::bound::BoundCreateStatement *>(bound.get());
    ASSERT_NE(create_stmt, nullptr);
    ASSERT_TRUE(std::holds_alternative<dreamdb::binder::bound::BoundCreateCollection>(create_stmt->create_operation()));

    const auto & op_bound = std::get<dreamdb::binder::bound::BoundCreateCollection>(create_stmt->create_operation());
    EXPECT_EQ(op_bound.collection_name, "test_collection");
    ASSERT_EQ(op_bound.column_definitions.size(), 2u);
    EXPECT_EQ(op_bound.column_definitions[0].name, "id");
    EXPECT_TRUE(op_bound.column_definitions[0].not_null.has_value());
    EXPECT_TRUE(op_bound.column_definitions[0].default_value != nullptr);
}

TEST_F(CreateBinderTest, BindCreateIndexRequiresCurrentDatabase)
{
    dreamdb::parser::ast::AstCreateIndex op;
    op.index_name = "idx1";
    op.collection_name = "test_collection";
    op.column_names = {"id"};
    op.index_type = std::make_optional<std::string>("BTREE");

    auto ast = dreamdb::parser::ast::AstCreateStatement::create(std::move(op), false, 1, 1);
    EXPECT_THROW((void)binder.bind(*ast), std::runtime_error);
}

TEST_F(CreateBinderTest, BindCreateIndex)
{
    auto use_stmt = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    ASSERT_NE(binder.bind(*use_stmt), nullptr);

    dreamdb::parser::ast::AstCreateIndex op;
    op.index_name = "idx1";
    op.collection_name = "test_collection";
    op.column_names = {"id"};
    op.index_type = std::make_optional<std::string>("HASH");

    auto ast = dreamdb::parser::ast::AstCreateStatement::create(std::move(op), false, 1, 1);
    auto bound = binder.bind(*ast);
    ASSERT_NE(bound, nullptr);

    auto * create_stmt = dynamic_cast<const dreamdb::binder::bound::BoundCreateStatement *>(bound.get());
    ASSERT_NE(create_stmt, nullptr);
    ASSERT_TRUE(std::holds_alternative<dreamdb::binder::bound::BoundCreateIndex>(create_stmt->create_operation()));

    const auto & op_bound = std::get<dreamdb::binder::bound::BoundCreateIndex>(create_stmt->create_operation());
    EXPECT_EQ(op_bound.collection_id, dreamdb::common::collection_id_t{99});
    EXPECT_EQ(op_bound.index_name, "idx1");
    ASSERT_EQ(op_bound.column_ids.size(), 1u);
    EXPECT_EQ(op_bound.column_ids[0], dreamdb::common::column_id_t{0});
    EXPECT_EQ(op_bound.index_type, dreamdb::common::IndexType::HASH);
}

TEST_F(CreateBinderTest, BindCreateIndexUnknownTypeThrows)
{
    auto use_stmt = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    ASSERT_NE(binder.bind(*use_stmt), nullptr);

    dreamdb::parser::ast::AstCreateIndex op;
    op.index_name = "idx1";
    op.collection_name = "test_collection";
    op.column_names = {"id"};
    op.index_type = std::make_optional<std::string>("UNKNOWN");

    auto ast = dreamdb::parser::ast::AstCreateStatement::create(std::move(op), false, 1, 1);
    EXPECT_THROW((void)binder.bind(*ast), std::runtime_error);
}

TEST_F(CreateBinderTest, BindCreateVIndexRequiresCurrentDatabase)
{
    dreamdb::parser::ast::AstCreateVIndex op;
    op.vindex_name = "v1";
    op.collection_name = "test_collection";
    op.column_name = "embedding";
    op.vindex_type = std::make_optional<std::string>("HNSW");

    auto ast = dreamdb::parser::ast::AstCreateStatement::create(std::move(op), false, 1, 1);
    EXPECT_THROW((void)binder.bind(*ast), std::runtime_error);
}

TEST_F(CreateBinderTest, BindCreateVIndexHnswWithOptions)
{
    auto use_stmt = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    ASSERT_NE(binder.bind(*use_stmt), nullptr);

    dreamdb::parser::ast::AstCreateVIndex op;
    op.vindex_name = "v1";
    op.collection_name = "test_collection";
    op.column_name = "embedding";
    op.vindex_type = std::make_optional<std::string>("HNSW");

    // WITH (m=16, ef_construction=200, metric='l2')
    op.with_clauses.emplace_back(
        "m",
        dreamdb::parser::ast::AstLiteralExpression::create_integer(16, 1, 1)
    );
    op.with_clauses.emplace_back(
        "ef_construction",
        dreamdb::parser::ast::AstLiteralExpression::create_integer(200, 1, 1)
    );
    op.with_clauses.emplace_back(
        "metric",
        dreamdb::parser::ast::AstLiteralExpression::create_string("l2", 1, 1)
    );

    auto ast = dreamdb::parser::ast::AstCreateStatement::create(std::move(op), false, 1, 1);
    auto bound = binder.bind(*ast);
    ASSERT_NE(bound, nullptr);

    auto * create_stmt = dynamic_cast<const dreamdb::binder::bound::BoundCreateStatement *>(bound.get());
    ASSERT_NE(create_stmt, nullptr);
    ASSERT_TRUE(std::holds_alternative<dreamdb::binder::bound::BoundCreateVIndex>(create_stmt->create_operation()));

    const auto & op_bound = std::get<dreamdb::binder::bound::BoundCreateVIndex>(create_stmt->create_operation());
    EXPECT_EQ(op_bound.collection_id, dreamdb::common::collection_id_t{99});
    EXPECT_EQ(op_bound.vindex_name, "v1");
    EXPECT_EQ(op_bound.column_id, dreamdb::common::column_id_t{10});
    EXPECT_EQ(op_bound.vindex_type, dreamdb::common::VIndexType::HNSW);
    EXPECT_TRUE(op_bound.with_options.m.has_value());
    EXPECT_EQ(op_bound.with_options.m.value(), 16u);
    EXPECT_TRUE(op_bound.with_options.ef_construction.has_value());
    EXPECT_EQ(op_bound.with_options.ef_construction.value(), 200u);
    EXPECT_TRUE(op_bound.with_options.metric.has_value());
    EXPECT_EQ(op_bound.with_options.metric.value(), dreamdb::common::MetricType::L2);
}

TEST_F(CreateBinderTest, BindCreateVIndexInvalidWithOptionThrows)
{
    auto use_stmt = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    ASSERT_NE(binder.bind(*use_stmt), nullptr);

    dreamdb::parser::ast::AstCreateVIndex op;
    op.vindex_name = "v1";
    op.collection_name = "test_collection";
    op.column_name = "embedding";
    op.vindex_type = std::make_optional<std::string>("IVF_FLAT");

    // IVF_FLAT 不允许 m
    op.with_clauses.emplace_back(
        "m",
        dreamdb::parser::ast::AstLiteralExpression::create_integer(16, 1, 1)
    );

    auto ast = dreamdb::parser::ast::AstCreateStatement::create(std::move(op), false, 1, 1);
    EXPECT_THROW((void)binder.bind(*ast), std::runtime_error);
}


