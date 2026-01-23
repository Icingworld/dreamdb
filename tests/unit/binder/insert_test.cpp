#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <dreamdb/binder/binder.h>
#include <dreamdb/binder/bound/debug/debug_printer.h>
#include <dreamdb/binder/bound/statement/statement.h>
#include <dreamdb/binder/bound/statement/insert.h>
#include <dreamdb/parser/ast/statement/use.h>
#include <dreamdb/parser/ast/statement/insert.h>
#include <dreamdb/parser/ast/expression/literal.h>

#include "catalog_mock.h"

using ::testing::Return;

class InsertBinderTest : public ::testing::Test
{
public:
    InsertBinderTest()
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
 * @brief 测试绑定 INSERT 语句（指定列名列表）
 */
TEST_F(InsertBinderTest, BindInsertWithColumnNames)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建列名列表和值列表
    std::vector<std::string> column_names{"id", "name", "age"};
    std::vector<std::unique_ptr<dreamdb::parser::ast::AstExpression>> values;
    values.push_back(dreamdb::parser::ast::AstLiteralExpression::create_integer(1, 1, 1));
    values.push_back(dreamdb::parser::ast::AstLiteralExpression::create_string("Alice", 1, 1));
    values.push_back(dreamdb::parser::ast::AstLiteralExpression::create_integer(25, 1, 1));

    // 创建 INSERT 语句
    auto ast_insert_statement = dreamdb::parser::ast::AstInsertStatement::create(
        "test_collection",
        std::move(column_names),
        std::move(values),
        1, 1
    );

    // 绑定语句
    auto bound_insert_statement = binder.bind(*ast_insert_statement);
    ASSERT_NE(bound_insert_statement, nullptr);

    // 验证语句类型
    EXPECT_EQ(bound_insert_statement->statement_type(), dreamdb::binder::bound::BoundStatementType::Insert);

    // 验证 BoundInsertStatement 的内容
    auto * insert_stmt = dynamic_cast<const dreamdb::binder::bound::BoundInsertStatement *>(bound_insert_statement.get());
    ASSERT_NE(insert_stmt, nullptr);
    EXPECT_EQ(insert_stmt->collection_id(), 99);
    EXPECT_EQ(insert_stmt->insert_item_count(), 3);

    // 验证格式化输出
    std::string formatted = printer.format(*bound_insert_statement);
    EXPECT_TRUE(formatted.find("INSERT") != std::string::npos);
    EXPECT_TRUE(formatted.find("collection_id:99") != std::string::npos);
}

/**
 * @brief 测试绑定 INSERT 语句（不指定列名列表，按表结构顺序）
 */
TEST_F(InsertBinderTest, BindInsertWithoutColumnNames)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建值列表（只提供所有列的值）
    std::vector<std::string> column_names;  // 空列名列表
    std::vector<std::unique_ptr<dreamdb::parser::ast::AstExpression>> values;
    values.push_back(dreamdb::parser::ast::AstLiteralExpression::create_integer(1, 1, 1));  // id
    values.push_back(dreamdb::parser::ast::AstLiteralExpression::create_string("Bob", 1, 1));  // name
    values.push_back(dreamdb::parser::ast::AstLiteralExpression::create_integer(30, 1, 1));  // age

    // 创建 INSERT 语句
    auto ast_insert_statement = dreamdb::parser::ast::AstInsertStatement::create(
        "test_collection",
        std::move(column_names),
        std::move(values),
        1, 1
    );

    // 绑定语句
    auto bound_insert_statement = binder.bind(*ast_insert_statement);
    ASSERT_NE(bound_insert_statement, nullptr);

    // 验证 BoundInsertStatement 的内容
    auto * insert_stmt = dynamic_cast<const dreamdb::binder::bound::BoundInsertStatement *>(bound_insert_statement.get());
    ASSERT_NE(insert_stmt, nullptr);
    EXPECT_EQ(insert_stmt->collection_id(), 99);
    EXPECT_EQ(insert_stmt->insert_item_count(), 3);
}

/**
 * @brief 测试绑定 INSERT 语句（不指定列名列表，只提供非空列的值）
 */
TEST_F(InsertBinderTest, BindInsertWithoutColumnNamesOnlyNonNullable)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建值列表（只提供非空列 id 的值）
    std::vector<std::string> column_names;  // 空列名列表
    std::vector<std::unique_ptr<dreamdb::parser::ast::AstExpression>> values;
    values.push_back(dreamdb::parser::ast::AstLiteralExpression::create_integer(2, 1, 1));  // id (NOT NULL)

    // 创建 INSERT 语句
    auto ast_insert_statement = dreamdb::parser::ast::AstInsertStatement::create(
        "test_collection",
        std::move(column_names),
        std::move(values),
        1, 1
    );

    // 绑定语句
    auto bound_insert_statement = binder.bind(*ast_insert_statement);
    ASSERT_NE(bound_insert_statement, nullptr);

    // 验证 BoundInsertStatement 的内容
    auto * insert_stmt = dynamic_cast<const dreamdb::binder::bound::BoundInsertStatement *>(bound_insert_statement.get());
    ASSERT_NE(insert_stmt, nullptr);
    EXPECT_EQ(insert_stmt->collection_id(), 99);
    EXPECT_EQ(insert_stmt->insert_item_count(), 1);  // 只有 id 列
}

/**
 * @brief 测试绑定 INSERT 语句（指定部分列名）
 */
TEST_F(InsertBinderTest, BindInsertWithPartialColumnNames)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建列名列表和值列表（只指定 id 和 name）
    std::vector<std::string> column_names{"id", "name"};
    std::vector<std::unique_ptr<dreamdb::parser::ast::AstExpression>> values;
    values.push_back(dreamdb::parser::ast::AstLiteralExpression::create_integer(3, 1, 1));
    values.push_back(dreamdb::parser::ast::AstLiteralExpression::create_string("Charlie", 1, 1));

    // 创建 INSERT 语句
    auto ast_insert_statement = dreamdb::parser::ast::AstInsertStatement::create(
        "test_collection",
        std::move(column_names),
        std::move(values),
        1, 1
    );

    // 绑定语句
    auto bound_insert_statement = binder.bind(*ast_insert_statement);
    ASSERT_NE(bound_insert_statement, nullptr);

    // 验证 BoundInsertStatement 的内容
    auto * insert_stmt = dynamic_cast<const dreamdb::binder::bound::BoundInsertStatement *>(bound_insert_statement.get());
    ASSERT_NE(insert_stmt, nullptr);
    EXPECT_EQ(insert_stmt->collection_id(), 99);
    EXPECT_EQ(insert_stmt->insert_item_count(), 2);  // id 和 name
}

/**
 * @brief 测试集合不存在的情况
 */
TEST_F(InsertBinderTest, BindInsertNonexistentCollection)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 INSERT 语句（不存在的集合）
    std::vector<std::string> column_names{"id"};
    std::vector<std::unique_ptr<dreamdb::parser::ast::AstExpression>> values;
    values.push_back(dreamdb::parser::ast::AstLiteralExpression::create_integer(1, 1, 1));

    auto ast_insert_statement = dreamdb::parser::ast::AstInsertStatement::create(
        "nonexistent_collection",
        std::move(column_names),
        std::move(values),
        1, 1
    );

    // 绑定语句应该抛出异常
    EXPECT_THROW(
        {
            auto bound_insert_statement = binder.bind(*ast_insert_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试未设置当前数据库的情况
 */
TEST_F(InsertBinderTest, BindInsertWithoutCurrentDatabase)
{
    // 不绑定 USE，直接尝试绑定 INSERT
    std::vector<std::string> column_names{"id"};
    std::vector<std::unique_ptr<dreamdb::parser::ast::AstExpression>> values;
    values.push_back(dreamdb::parser::ast::AstLiteralExpression::create_integer(1, 1, 1));

    auto ast_insert_statement = dreamdb::parser::ast::AstInsertStatement::create(
        "test_collection",
        std::move(column_names),
        std::move(values),
        1, 1
    );

    // 绑定语句应该抛出异常
    EXPECT_THROW(
        {
            auto bound_insert_statement = binder.bind(*ast_insert_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试列名不存在的情况
 */
TEST_F(InsertBinderTest, BindInsertNonexistentColumn)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 INSERT 语句（不存在的列）
    std::vector<std::string> column_names{"nonexistent_column"};
    std::vector<std::unique_ptr<dreamdb::parser::ast::AstExpression>> values;
    values.push_back(dreamdb::parser::ast::AstLiteralExpression::create_integer(1, 1, 1));

    auto ast_insert_statement = dreamdb::parser::ast::AstInsertStatement::create(
        "test_collection",
        std::move(column_names),
        std::move(values),
        1, 1
    );

    // 绑定语句应该抛出异常
    EXPECT_THROW(
        {
            auto bound_insert_statement = binder.bind(*ast_insert_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试列名重复的情况
 */
TEST_F(InsertBinderTest, BindInsertDuplicateColumn)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 INSERT 语句（重复的列）
    std::vector<std::string> column_names{"id", "id"};  // 重复的列
    std::vector<std::unique_ptr<dreamdb::parser::ast::AstExpression>> values;
    values.push_back(dreamdb::parser::ast::AstLiteralExpression::create_integer(1, 1, 1));
    values.push_back(dreamdb::parser::ast::AstLiteralExpression::create_integer(2, 1, 1));

    auto ast_insert_statement = dreamdb::parser::ast::AstInsertStatement::create(
        "test_collection",
        std::move(column_names),
        std::move(values),
        1, 1
    );

    // 绑定语句应该抛出异常
    EXPECT_THROW(
        {
            auto bound_insert_statement = binder.bind(*ast_insert_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试 NOT NULL 列未提供值的情况（指定列名时）
 */
TEST_F(InsertBinderTest, BindInsertMissingNotNullColumn)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 INSERT 语句（未提供 NOT NULL 列 id 的值）
    std::vector<std::string> column_names{"name", "age"};  // 缺少 id
    std::vector<std::unique_ptr<dreamdb::parser::ast::AstExpression>> values;
    values.push_back(dreamdb::parser::ast::AstLiteralExpression::create_string("David", 1, 1));
    values.push_back(dreamdb::parser::ast::AstLiteralExpression::create_integer(35, 1, 1));

    auto ast_insert_statement = dreamdb::parser::ast::AstInsertStatement::create(
        "test_collection",
        std::move(column_names),
        std::move(values),
        1, 1
    );

    // 绑定语句应该抛出异常
    EXPECT_THROW(
        {
            auto bound_insert_statement = binder.bind(*ast_insert_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试值数量不足的情况（不指定列名时）
 * @note 这个测试实际上无法创建，因为 AstInsertStatement 不允许空值列表
 * 但我们可以测试值数量为 0 的情况，这会在 binder 中检测到
 */
TEST_F(InsertBinderTest, BindInsertInsufficientValues)
{
    // 这个测试被跳过，因为 AstInsertStatement 构造函数要求值列表不能为空
    // 值数量不足的情况会在 binder 的验证逻辑中被检测到
    // 实际的值数量验证已经在其他测试中覆盖（如 BindInsertInvalidValueCount）
    GTEST_SKIP() << "AstInsertStatement requires non-empty values, insufficient values are checked in binder logic";
}

/**
 * @brief 测试值数量过多的情况（不指定列名时）
 */
TEST_F(InsertBinderTest, BindInsertTooManyValues)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建值列表（值数量超过列数量）
    std::vector<std::string> column_names;  // 空列名列表
    std::vector<std::unique_ptr<dreamdb::parser::ast::AstExpression>> values;
    values.push_back(dreamdb::parser::ast::AstLiteralExpression::create_integer(1, 1, 1));
    values.push_back(dreamdb::parser::ast::AstLiteralExpression::create_string("Eve", 1, 1));
    values.push_back(dreamdb::parser::ast::AstLiteralExpression::create_integer(40, 1, 1));
    values.push_back(dreamdb::parser::ast::AstLiteralExpression::create_integer(999, 1, 1));  // 多余的值

    auto ast_insert_statement = dreamdb::parser::ast::AstInsertStatement::create(
        "test_collection",
        std::move(column_names),
        std::move(values),
        1, 1
    );

    // 绑定语句应该抛出异常
    EXPECT_THROW(
        {
            auto bound_insert_statement = binder.bind(*ast_insert_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试值数量不匹配的情况（不指定列名时，既不是非空列数也不是总列数）
 */
TEST_F(InsertBinderTest, BindInsertInvalidValueCount)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建值列表（2 个值，既不是 1 个非空列，也不是 3 个总列）
    std::vector<std::string> column_names;  // 空列名列表
    std::vector<std::unique_ptr<dreamdb::parser::ast::AstExpression>> values;
    values.push_back(dreamdb::parser::ast::AstLiteralExpression::create_integer(1, 1, 1));
    values.push_back(dreamdb::parser::ast::AstLiteralExpression::create_string("Frank", 1, 1));

    auto ast_insert_statement = dreamdb::parser::ast::AstInsertStatement::create(
        "test_collection",
        std::move(column_names),
        std::move(values),
        1, 1
    );

    // 绑定语句应该抛出异常
    EXPECT_THROW(
        {
            auto bound_insert_statement = binder.bind(*ast_insert_statement);
        },
        std::runtime_error
    );
}

