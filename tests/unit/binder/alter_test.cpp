#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <dreamdb/binder/binder.h>
#include <dreamdb/binder/bound/debug/debug_printer.h>
#include <dreamdb/binder/bound/statement/statement.h>
#include <dreamdb/binder/bound/statement/alter.h>
#include <dreamdb/parser/ast/statement/use.h>
#include <dreamdb/parser/ast/statement/alter.h>
#include <dreamdb/parser/ast/statement/column_definition.h>
#include <dreamdb/parser/ast/expression/literal.h>

#include "catalog_mock.h"

using ::testing::Return;

class AlterBinderTest : public ::testing::Test
{
public:
    AlterBinderTest()
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
            false,  // is_nullable (NOT NULL)
            true    // is_insertable
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
 * @brief 测试绑定 ALTER ADD COLUMN 语句
 */
TEST_F(AlterBinderTest, BindAlterAddColumn)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建列定义：email VARCHAR(255)
    std::vector<std::unique_ptr<dreamdb::parser::ast::AstExpression>> type_args;
    type_args.push_back(dreamdb::parser::ast::AstLiteralExpression::create_integer(255, 1, 1));
    
    std::vector<dreamdb::parser::ast::AstColumnModifier> modifiers;
    dreamdb::parser::ast::AstColumnDefinition column_def(
        "email",
        "VARCHAR",
        std::move(type_args),
        std::move(modifiers),
        nullptr,  // 无默认值
        std::nullopt  // 无注释
    );

    // 创建 ALTER ADD COLUMN 操作
    dreamdb::parser::ast::AstAlterAddColumn add_column_op{std::move(column_def)};

    // 创建 ALTER 语句
    auto ast_alter_statement = dreamdb::parser::ast::AstAlterStatement::create(
        "test_collection",
        std::move(add_column_op),
        1, 1
    );

    // 绑定语句
    auto bound_alter_statement = binder.bind(*ast_alter_statement);
    ASSERT_NE(bound_alter_statement, nullptr);

    // 验证语句类型
    EXPECT_EQ(bound_alter_statement->statement_type(), dreamdb::binder::bound::BoundStatementType::Alter);

    // 验证 BoundAlterStatement 的内容
    auto * alter_stmt = dynamic_cast<const dreamdb::binder::bound::BoundAlterStatement *>(bound_alter_statement.get());
    ASSERT_NE(alter_stmt, nullptr);
    EXPECT_EQ(alter_stmt->collection_id(), 99);

    // 验证操作类型
    const auto & operation = alter_stmt->alter_operation();
    EXPECT_TRUE(std::holds_alternative<dreamdb::binder::bound::BoundAlterAddColumn>(operation));
    
    const auto & add_column = std::get<dreamdb::binder::bound::BoundAlterAddColumn>(operation);
    EXPECT_EQ(add_column.column_definition.name, "email");
}

/**
 * @brief 测试绑定 ALTER ADD COLUMN 语句（带默认值）
 */
TEST_F(AlterBinderTest, BindAlterAddColumnWithDefault)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建列定义：status INTEGER DEFAULT 0
    std::vector<std::unique_ptr<dreamdb::parser::ast::AstExpression>> type_args;
    std::vector<dreamdb::parser::ast::AstColumnModifier> modifiers;
    auto default_value = dreamdb::parser::ast::AstLiteralExpression::create_integer(0, 1, 1);
    
    dreamdb::parser::ast::AstColumnDefinition column_def(
        "status",
        "INTEGER",
        std::move(type_args),
        std::move(modifiers),
        std::move(default_value),
        std::nullopt
    );

    // 创建 ALTER ADD COLUMN 操作
    dreamdb::parser::ast::AstAlterAddColumn add_column_op{std::move(column_def)};

    // 创建 ALTER 语句
    auto ast_alter_statement = dreamdb::parser::ast::AstAlterStatement::create(
        "test_collection",
        std::move(add_column_op),
        1, 1
    );

    // 绑定语句
    auto bound_alter_statement = binder.bind(*ast_alter_statement);
    ASSERT_NE(bound_alter_statement, nullptr);

    // 验证 BoundAlterStatement 的内容
    auto * alter_stmt = dynamic_cast<const dreamdb::binder::bound::BoundAlterStatement *>(bound_alter_statement.get());
    ASSERT_NE(alter_stmt, nullptr);
    
    const auto & operation = alter_stmt->alter_operation();
    const auto & add_column = std::get<dreamdb::binder::bound::BoundAlterAddColumn>(operation);
    EXPECT_EQ(add_column.column_definition.name, "status");
    EXPECT_NE(add_column.column_definition.default_value, nullptr);
}

/**
 * @brief 测试绑定 ALTER DROP COLUMN 语句
 */
TEST_F(AlterBinderTest, BindAlterDropColumn)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 ALTER DROP COLUMN 操作
    dreamdb::parser::ast::AstAlterDropColumn drop_column_op{"name"};

    // 创建 ALTER 语句
    auto ast_alter_statement = dreamdb::parser::ast::AstAlterStatement::create(
        "test_collection",
        std::move(drop_column_op),
        1, 1
    );

    // 绑定语句
    auto bound_alter_statement = binder.bind(*ast_alter_statement);
    ASSERT_NE(bound_alter_statement, nullptr);

    // 验证语句类型
    EXPECT_EQ(bound_alter_statement->statement_type(), dreamdb::binder::bound::BoundStatementType::Alter);

    // 验证 BoundAlterStatement 的内容
    auto * alter_stmt = dynamic_cast<const dreamdb::binder::bound::BoundAlterStatement *>(bound_alter_statement.get());
    ASSERT_NE(alter_stmt, nullptr);
    EXPECT_EQ(alter_stmt->collection_id(), 99);

    // 验证操作类型
    const auto & operation = alter_stmt->alter_operation();
    EXPECT_TRUE(std::holds_alternative<dreamdb::binder::bound::BoundAlterDropColumn>(operation));
    
    const auto & drop_column = std::get<dreamdb::binder::bound::BoundAlterDropColumn>(operation);
    EXPECT_EQ(drop_column.column_id, dreamdb::common::column_id_t{1});  // name 列的 ID
}

/**
 * @brief 测试绑定 ALTER MODIFY COLUMN 语句
 */
TEST_F(AlterBinderTest, BindAlterModifyColumn)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建新的列定义：name VARCHAR(100) NOT NULL
    std::vector<std::unique_ptr<dreamdb::parser::ast::AstExpression>> type_args;
    type_args.push_back(dreamdb::parser::ast::AstLiteralExpression::create_integer(100, 1, 1));
    
    std::vector<dreamdb::parser::ast::AstColumnModifier> modifiers;
    modifiers.push_back(dreamdb::parser::ast::AstColumnModifier::NotNull);
    
    dreamdb::parser::ast::AstColumnDefinition new_column_def(
        "name",
        "VARCHAR",
        std::move(type_args),
        std::move(modifiers),
        nullptr,
        std::nullopt
    );

    // 创建 ALTER MODIFY COLUMN 操作
    dreamdb::parser::ast::AstAlterModifyColumn modify_column_op{std::move(new_column_def)};

    // 创建 ALTER 语句
    auto ast_alter_statement = dreamdb::parser::ast::AstAlterStatement::create(
        "test_collection",
        std::move(modify_column_op),
        1, 1
    );

    // 绑定语句
    auto bound_alter_statement = binder.bind(*ast_alter_statement);
    ASSERT_NE(bound_alter_statement, nullptr);

    // 验证 BoundAlterStatement 的内容
    auto * alter_stmt = dynamic_cast<const dreamdb::binder::bound::BoundAlterStatement *>(bound_alter_statement.get());
    ASSERT_NE(alter_stmt, nullptr);
    EXPECT_EQ(alter_stmt->collection_id(), 99);

    // 验证操作类型
    const auto & operation = alter_stmt->alter_operation();
    EXPECT_TRUE(std::holds_alternative<dreamdb::binder::bound::BoundAlterModifyColumn>(operation));
    
    const auto & modify_column = std::get<dreamdb::binder::bound::BoundAlterModifyColumn>(operation);
    EXPECT_EQ(modify_column.column_id, dreamdb::common::column_id_t{1});  // name 列的 ID
    EXPECT_EQ(modify_column.new_definition.name, "name");
}

/**
 * @brief 测试绑定 ALTER RENAME COLUMN 语句
 */
TEST_F(AlterBinderTest, BindAlterRenameColumn)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 ALTER RENAME COLUMN 操作
    dreamdb::parser::ast::AstAlterRenameColumn rename_column_op{"name", "full_name"};

    // 创建 ALTER 语句
    auto ast_alter_statement = dreamdb::parser::ast::AstAlterStatement::create(
        "test_collection",
        std::move(rename_column_op),
        1, 1
    );

    // 绑定语句
    auto bound_alter_statement = binder.bind(*ast_alter_statement);
    ASSERT_NE(bound_alter_statement, nullptr);

    // 验证语句类型
    EXPECT_EQ(bound_alter_statement->statement_type(), dreamdb::binder::bound::BoundStatementType::Alter);

    // 验证 BoundAlterStatement 的内容
    auto * alter_stmt = dynamic_cast<const dreamdb::binder::bound::BoundAlterStatement *>(bound_alter_statement.get());
    ASSERT_NE(alter_stmt, nullptr);
    EXPECT_EQ(alter_stmt->collection_id(), 99);

    // 验证操作类型
    const auto & operation = alter_stmt->alter_operation();
    EXPECT_TRUE(std::holds_alternative<dreamdb::binder::bound::BoundAlterRenameColumn>(operation));
    
    const auto & rename_column = std::get<dreamdb::binder::bound::BoundAlterRenameColumn>(operation);
    EXPECT_EQ(rename_column.column_id, dreamdb::common::column_id_t{1});  // name 列的 ID
    EXPECT_EQ(rename_column.new_name, "full_name");
}

/**
 * @brief 测试集合不存在的情况
 */
TEST_F(AlterBinderTest, BindAlterNonexistentCollection)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 ALTER DROP COLUMN 操作
    dreamdb::parser::ast::AstAlterDropColumn drop_column_op{"name"};

    // 创建 ALTER 语句（不存在的集合）
    auto ast_alter_statement = dreamdb::parser::ast::AstAlterStatement::create(
        "nonexistent_collection",
        std::move(drop_column_op),
        1, 1
    );

    // 绑定语句应该抛出异常
    EXPECT_THROW(
        {
            auto bound_alter_statement = binder.bind(*ast_alter_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试未设置当前数据库的情况
 */
TEST_F(AlterBinderTest, BindAlterWithoutCurrentDatabase)
{
    // 不绑定 USE，直接尝试绑定 ALTER
    dreamdb::parser::ast::AstAlterDropColumn drop_column_op{"name"};

    auto ast_alter_statement = dreamdb::parser::ast::AstAlterStatement::create(
        "test_collection",
        std::move(drop_column_op),
        1, 1
    );

    // 绑定语句应该抛出异常
    EXPECT_THROW(
        {
            auto bound_alter_statement = binder.bind(*ast_alter_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试 DROP COLUMN 时列不存在的情况
 */
TEST_F(AlterBinderTest, BindAlterDropColumnNonexistent)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 ALTER DROP COLUMN 操作（不存在的列）
    dreamdb::parser::ast::AstAlterDropColumn drop_column_op{"nonexistent_column"};

    // 创建 ALTER 语句
    auto ast_alter_statement = dreamdb::parser::ast::AstAlterStatement::create(
        "test_collection",
        std::move(drop_column_op),
        1, 1
    );

    // 绑定语句应该抛出异常
    EXPECT_THROW(
        {
            auto bound_alter_statement = binder.bind(*ast_alter_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试 MODIFY COLUMN 时列不存在的情况
 */
TEST_F(AlterBinderTest, BindAlterModifyColumnNonexistent)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建新的列定义（不存在的列）
    std::vector<std::unique_ptr<dreamdb::parser::ast::AstExpression>> type_args;
    std::vector<dreamdb::parser::ast::AstColumnModifier> modifiers;
    
    dreamdb::parser::ast::AstColumnDefinition new_column_def(
        "nonexistent_column",
        "INTEGER",
        std::move(type_args),
        std::move(modifiers),
        nullptr,
        std::nullopt
    );

    // 创建 ALTER MODIFY COLUMN 操作
    dreamdb::parser::ast::AstAlterModifyColumn modify_column_op{std::move(new_column_def)};

    // 创建 ALTER 语句
    auto ast_alter_statement = dreamdb::parser::ast::AstAlterStatement::create(
        "test_collection",
        std::move(modify_column_op),
        1, 1
    );

    // 绑定语句应该抛出异常
    EXPECT_THROW(
        {
            auto bound_alter_statement = binder.bind(*ast_alter_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试 RENAME COLUMN 时列不存在的情况
 */
TEST_F(AlterBinderTest, BindAlterRenameColumnNonexistent)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 ALTER RENAME COLUMN 操作（不存在的列）
    dreamdb::parser::ast::AstAlterRenameColumn rename_column_op{"nonexistent_column", "new_name"};

    // 创建 ALTER 语句
    auto ast_alter_statement = dreamdb::parser::ast::AstAlterStatement::create(
        "test_collection",
        std::move(rename_column_op),
        1, 1
    );

    // 绑定语句应该抛出异常
    EXPECT_THROW(
        {
            auto bound_alter_statement = binder.bind(*ast_alter_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试绑定 ALTER ADD COLUMN 语句（带 NOT NULL 修饰符）
 */
TEST_F(AlterBinderTest, BindAlterAddColumnWithNotNull)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建列定义：phone VARCHAR(20) NOT NULL
    std::vector<std::unique_ptr<dreamdb::parser::ast::AstExpression>> type_args;
    type_args.push_back(dreamdb::parser::ast::AstLiteralExpression::create_integer(20, 1, 1));
    
    std::vector<dreamdb::parser::ast::AstColumnModifier> modifiers;
    modifiers.push_back(dreamdb::parser::ast::AstColumnModifier::NotNull);
    
    dreamdb::parser::ast::AstColumnDefinition column_def(
        "phone",
        "VARCHAR",
        std::move(type_args),
        std::move(modifiers),
        nullptr,
        std::nullopt
    );

    // 创建 ALTER ADD COLUMN 操作
    dreamdb::parser::ast::AstAlterAddColumn add_column_op{std::move(column_def)};

    // 创建 ALTER 语句
    auto ast_alter_statement = dreamdb::parser::ast::AstAlterStatement::create(
        "test_collection",
        std::move(add_column_op),
        1, 1
    );

    // 绑定语句
    auto bound_alter_statement = binder.bind(*ast_alter_statement);
    ASSERT_NE(bound_alter_statement, nullptr);

    // 验证 BoundAlterStatement 的内容
    auto * alter_stmt = dynamic_cast<const dreamdb::binder::bound::BoundAlterStatement *>(bound_alter_statement.get());
    ASSERT_NE(alter_stmt, nullptr);
    
    const auto & operation = alter_stmt->alter_operation();
    const auto & add_column = std::get<dreamdb::binder::bound::BoundAlterAddColumn>(operation);
    EXPECT_EQ(add_column.column_definition.name, "phone");
    EXPECT_TRUE(add_column.column_definition.not_null.has_value());
    EXPECT_TRUE(add_column.column_definition.not_null.value());
}

/**
 * @brief 测试绑定 ALTER ADD COLUMN 语句（带多个修饰符）
 */
TEST_F(AlterBinderTest, BindAlterAddColumnWithMultipleModifiers)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建列定义：code VARCHAR(10) NOT NULL UNIQUE
    std::vector<std::unique_ptr<dreamdb::parser::ast::AstExpression>> type_args;
    type_args.push_back(dreamdb::parser::ast::AstLiteralExpression::create_integer(10, 1, 1));
    
    std::vector<dreamdb::parser::ast::AstColumnModifier> modifiers;
    modifiers.push_back(dreamdb::parser::ast::AstColumnModifier::NotNull);
    modifiers.push_back(dreamdb::parser::ast::AstColumnModifier::Unique);
    
    dreamdb::parser::ast::AstColumnDefinition column_def(
        "code",
        "VARCHAR",
        std::move(type_args),
        std::move(modifiers),
        nullptr,
        std::nullopt
    );

    // 创建 ALTER ADD COLUMN 操作
    dreamdb::parser::ast::AstAlterAddColumn add_column_op{std::move(column_def)};

    // 创建 ALTER 语句
    auto ast_alter_statement = dreamdb::parser::ast::AstAlterStatement::create(
        "test_collection",
        std::move(add_column_op),
        1, 1
    );

    // 绑定语句
    auto bound_alter_statement = binder.bind(*ast_alter_statement);
    ASSERT_NE(bound_alter_statement, nullptr);

    // 验证 BoundAlterStatement 的内容
    auto * alter_stmt = dynamic_cast<const dreamdb::binder::bound::BoundAlterStatement *>(bound_alter_statement.get());
    ASSERT_NE(alter_stmt, nullptr);
    
    const auto & operation = alter_stmt->alter_operation();
    const auto & add_column = std::get<dreamdb::binder::bound::BoundAlterAddColumn>(operation);
    EXPECT_EQ(add_column.column_definition.name, "code");
    EXPECT_TRUE(add_column.column_definition.not_null.has_value());
    EXPECT_TRUE(add_column.column_definition.not_null.value());
    EXPECT_TRUE(add_column.column_definition.unique.has_value());
    EXPECT_TRUE(add_column.column_definition.unique.value());
}

