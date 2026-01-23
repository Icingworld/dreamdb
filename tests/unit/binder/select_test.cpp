#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <dreamdb/binder/binder.h>
#include <dreamdb/binder/bound/debug/debug_printer.h>
#include <dreamdb/binder/bound/statement/statement.h>
#include <dreamdb/binder/bound/statement/select.h>
#include <dreamdb/parser/ast/statement/use.h>
#include <dreamdb/parser/ast/statement/select.h>
#include <dreamdb/parser/ast/expression/literal.h>
#include <dreamdb/parser/ast/expression/column_reference.h>
#include <dreamdb/parser/ast/expression/binary.h>
#include <dreamdb/common/type.h>

#include "catalog_mock.h"

using ::testing::Return;

class SelectBinderTest : public ::testing::Test
{
public:
    SelectBinderTest()
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
 * @brief 测试绑定 SELECT * 语句
 */
TEST_F(SelectBinderTest, BindSelectStar)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 SELECT * 语句
    std::vector<dreamdb::parser::ast::AstSelectItem> select_items;
    select_items.push_back(dreamdb::parser::ast::AstSelectStarItem{});

    auto ast_select_statement = dreamdb::parser::ast::AstSelectStatement::create(
        "test_collection",
        std::move(select_items),
        nullptr,  // 无 WHERE 子句
        {},       // 无 GROUP BY 子句
        nullptr,  // 无 HAVING 子句
        {},       // 无 ORDER BY 子句
        std::nullopt,  // 无 LIMIT
        std::nullopt,  // 无 OFFSET
        1, 1
    );

    // 绑定语句
    auto bound_select_statement = binder.bind(*ast_select_statement);
    ASSERT_NE(bound_select_statement, nullptr);

    // 验证语句类型
    EXPECT_EQ(bound_select_statement->statement_type(), dreamdb::binder::bound::BoundStatementType::Select);

    // 验证 BoundSelectStatement 的内容
    auto * select_stmt = dynamic_cast<const dreamdb::binder::bound::BoundSelectStatement *>(bound_select_statement.get());
    ASSERT_NE(select_stmt, nullptr);
    EXPECT_EQ(select_stmt->collection_id(), 99);
    EXPECT_EQ(select_stmt->select_item_count(), 3);  // SELECT * 展开为所有列
    EXPECT_FALSE(select_stmt->has_where());
    EXPECT_EQ(select_stmt->group_by_count(), 0);
    EXPECT_FALSE(select_stmt->has_having());
    EXPECT_EQ(select_stmt->order_by_count(), 0);
    EXPECT_FALSE(select_stmt->has_limit());
    EXPECT_FALSE(select_stmt->has_offset());
}

/**
 * @brief 测试绑定 SELECT 列名语句
 */
TEST_F(SelectBinderTest, BindSelectColumn)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 SELECT 列名语句
    std::vector<dreamdb::parser::ast::AstSelectItem> select_items;
    auto id_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(
        std::move(id_column), std::nullopt
    ));

    auto ast_select_statement = dreamdb::parser::ast::AstSelectStatement::create(
        "test_collection",
        std::move(select_items),
        nullptr,
        {},
        nullptr,
        {},
        std::nullopt,
        std::nullopt,
        1, 1
    );

    // 绑定语句
    auto bound_select_statement = binder.bind(*ast_select_statement);
    ASSERT_NE(bound_select_statement, nullptr);

    // 验证 BoundSelectStatement 的内容
    auto * select_stmt = dynamic_cast<const dreamdb::binder::bound::BoundSelectStatement *>(bound_select_statement.get());
    ASSERT_NE(select_stmt, nullptr);
    EXPECT_EQ(select_stmt->collection_id(), 99);
    EXPECT_EQ(select_stmt->select_item_count(), 1);
}

/**
 * @brief 测试绑定 SELECT 多个列名语句
 */
TEST_F(SelectBinderTest, BindSelectMultipleColumns)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 SELECT 多个列名语句
    std::vector<dreamdb::parser::ast::AstSelectItem> select_items;
    auto id_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(
        std::move(id_column), std::nullopt
    ));
    auto name_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("name", 1, 1);
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(
        std::move(name_column), std::nullopt
    ));

    auto ast_select_statement = dreamdb::parser::ast::AstSelectStatement::create(
        "test_collection",
        std::move(select_items),
        nullptr,
        {},
        nullptr,
        {},
        std::nullopt,
        std::nullopt,
        1, 1
    );

    // 绑定语句
    auto bound_select_statement = binder.bind(*ast_select_statement);
    ASSERT_NE(bound_select_statement, nullptr);

    // 验证 BoundSelectStatement 的内容
    auto * select_stmt = dynamic_cast<const dreamdb::binder::bound::BoundSelectStatement *>(bound_select_statement.get());
    ASSERT_NE(select_stmt, nullptr);
    EXPECT_EQ(select_stmt->select_item_count(), 2);
}

/**
 * @brief 测试绑定 SELECT 带别名语句
 */
TEST_F(SelectBinderTest, BindSelectWithAlias)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 SELECT 带别名语句
    std::vector<dreamdb::parser::ast::AstSelectItem> select_items;
    auto name_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("name", 1, 1);
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(
        std::move(name_column), std::make_optional<std::string>("user_name")
    ));

    auto ast_select_statement = dreamdb::parser::ast::AstSelectStatement::create(
        "test_collection",
        std::move(select_items),
        nullptr,
        {},
        nullptr,
        {},
        std::nullopt,
        std::nullopt,
        1, 1
    );

    // 绑定语句
    auto bound_select_statement = binder.bind(*ast_select_statement);
    ASSERT_NE(bound_select_statement, nullptr);

    // 验证 BoundSelectStatement 的内容
    auto * select_stmt = dynamic_cast<const dreamdb::binder::bound::BoundSelectStatement *>(bound_select_statement.get());
    ASSERT_NE(select_stmt, nullptr);
    EXPECT_EQ(select_stmt->select_item_count(), 1);
    EXPECT_EQ(select_stmt->select_item_at(0).alias, "user_name");
}

/**
 * @brief 测试绑定 SELECT 带 WHERE 子句语句
 */
TEST_F(SelectBinderTest, BindSelectWithWhere)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 SELECT 语句
    std::vector<dreamdb::parser::ast::AstSelectItem> select_items;
    auto id_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(
        std::move(id_column), std::nullopt
    ));

    // 创建 WHERE 子句：id = 1
    auto where_id_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    auto literal_one = dreamdb::parser::ast::AstLiteralExpression::create_integer(1, 1, 1);
    auto where_expr = dreamdb::parser::ast::AstBinaryExpression::create(
        dreamdb::parser::ast::AstBinaryOperatorType::Equal,
        std::move(where_id_column),
        std::move(literal_one),
        1, 1
    );

    auto ast_select_statement = dreamdb::parser::ast::AstSelectStatement::create(
        "test_collection",
        std::move(select_items),
        std::move(where_expr),
        {},
        nullptr,
        {},
        std::nullopt,
        std::nullopt,
        1, 1
    );

    // 绑定语句
    auto bound_select_statement = binder.bind(*ast_select_statement);
    ASSERT_NE(bound_select_statement, nullptr);

    // 验证 BoundSelectStatement 的内容
    auto * select_stmt = dynamic_cast<const dreamdb::binder::bound::BoundSelectStatement *>(bound_select_statement.get());
    ASSERT_NE(select_stmt, nullptr);
    EXPECT_TRUE(select_stmt->has_where());
    ASSERT_NE(select_stmt->where(), nullptr);
}

/**
 * @brief 测试绑定 SELECT 带 ORDER BY 子句语句
 */
TEST_F(SelectBinderTest, BindSelectWithOrderBy)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 SELECT 语句
    std::vector<dreamdb::parser::ast::AstSelectItem> select_items;
    auto id_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(
        std::move(id_column), std::nullopt
    ));

    // 创建 ORDER BY 子句：ORDER BY id ASC
    std::vector<dreamdb::parser::ast::AstOrderByItem> order_by_items;
    auto order_by_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    order_by_items.push_back(dreamdb::parser::ast::AstOrderByItem(
        std::move(order_by_column), dreamdb::common::Direction::ASC
    ));

    auto ast_select_statement = dreamdb::parser::ast::AstSelectStatement::create(
        "test_collection",
        std::move(select_items),
        nullptr,
        {},
        nullptr,
        std::move(order_by_items),
        std::nullopt,
        std::nullopt,
        1, 1
    );

    // 绑定语句
    auto bound_select_statement = binder.bind(*ast_select_statement);
    ASSERT_NE(bound_select_statement, nullptr);

    // 验证 BoundSelectStatement 的内容
    auto * select_stmt = dynamic_cast<const dreamdb::binder::bound::BoundSelectStatement *>(bound_select_statement.get());
    ASSERT_NE(select_stmt, nullptr);
    EXPECT_EQ(select_stmt->order_by_count(), 1);
    EXPECT_EQ(select_stmt->order_by_at(0).direction, dreamdb::common::Direction::ASC);
}

/**
 * @brief 测试绑定 SELECT 带 ORDER BY DESC 子句语句
 */
TEST_F(SelectBinderTest, BindSelectWithOrderByDesc)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 SELECT 语句
    std::vector<dreamdb::parser::ast::AstSelectItem> select_items;
    auto name_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("name", 1, 1);
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(
        std::move(name_column), std::nullopt
    ));

    // 创建 ORDER BY 子句：ORDER BY name DESC
    std::vector<dreamdb::parser::ast::AstOrderByItem> order_by_items;
    auto order_by_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("name", 1, 1);
    order_by_items.push_back(dreamdb::parser::ast::AstOrderByItem(
        std::move(order_by_column), dreamdb::common::Direction::DESC
    ));

    auto ast_select_statement = dreamdb::parser::ast::AstSelectStatement::create(
        "test_collection",
        std::move(select_items),
        nullptr,
        {},
        nullptr,
        std::move(order_by_items),
        std::nullopt,
        std::nullopt,
        1, 1
    );

    // 绑定语句
    auto bound_select_statement = binder.bind(*ast_select_statement);
    ASSERT_NE(bound_select_statement, nullptr);

    // 验证 BoundSelectStatement 的内容
    auto * select_stmt = dynamic_cast<const dreamdb::binder::bound::BoundSelectStatement *>(bound_select_statement.get());
    ASSERT_NE(select_stmt, nullptr);
    EXPECT_EQ(select_stmt->order_by_count(), 1);
    EXPECT_EQ(select_stmt->order_by_at(0).direction, dreamdb::common::Direction::DESC);
}

/**
 * @brief 测试绑定 SELECT 带 LIMIT 子句语句
 */
TEST_F(SelectBinderTest, BindSelectWithLimit)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 SELECT 语句
    std::vector<dreamdb::parser::ast::AstSelectItem> select_items;
    auto id_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(
        std::move(id_column), std::nullopt
    ));

    auto ast_select_statement = dreamdb::parser::ast::AstSelectStatement::create(
        "test_collection",
        std::move(select_items),
        nullptr,
        {},
        nullptr,
        {},
        std::make_optional<std::uint64_t>(10),  // LIMIT 10
        std::nullopt,
        1, 1
    );

    // 绑定语句
    auto bound_select_statement = binder.bind(*ast_select_statement);
    ASSERT_NE(bound_select_statement, nullptr);

    // 验证 BoundSelectStatement 的内容
    auto * select_stmt = dynamic_cast<const dreamdb::binder::bound::BoundSelectStatement *>(bound_select_statement.get());
    ASSERT_NE(select_stmt, nullptr);
    EXPECT_TRUE(select_stmt->has_limit());
    EXPECT_EQ(select_stmt->limit(), 10);
}

/**
 * @brief 测试绑定 SELECT 带 OFFSET 子句语句
 */
TEST_F(SelectBinderTest, BindSelectWithOffset)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 SELECT 语句
    std::vector<dreamdb::parser::ast::AstSelectItem> select_items;
    auto id_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(
        std::move(id_column), std::nullopt
    ));

    auto ast_select_statement = dreamdb::parser::ast::AstSelectStatement::create(
        "test_collection",
        std::move(select_items),
        nullptr,
        {},
        nullptr,
        {},
        std::nullopt,
        std::make_optional<std::uint64_t>(5),  // OFFSET 5
        1, 1
    );

    // 绑定语句
    auto bound_select_statement = binder.bind(*ast_select_statement);
    ASSERT_NE(bound_select_statement, nullptr);

    // 验证 BoundSelectStatement 的内容
    auto * select_stmt = dynamic_cast<const dreamdb::binder::bound::BoundSelectStatement *>(bound_select_statement.get());
    ASSERT_NE(select_stmt, nullptr);
    EXPECT_TRUE(select_stmt->has_offset());
    EXPECT_EQ(select_stmt->offset(), 5);
}

/**
 * @brief 测试绑定 SELECT 带 LIMIT 和 OFFSET 子句语句
 */
TEST_F(SelectBinderTest, BindSelectWithLimitAndOffset)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 SELECT 语句
    std::vector<dreamdb::parser::ast::AstSelectItem> select_items;
    auto id_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(
        std::move(id_column), std::nullopt
    ));

    auto ast_select_statement = dreamdb::parser::ast::AstSelectStatement::create(
        "test_collection",
        std::move(select_items),
        nullptr,
        {},
        nullptr,
        {},
        std::make_optional<std::uint64_t>(20),  // LIMIT 20
        std::make_optional<std::uint64_t>(10),  // OFFSET 10
        1, 1
    );

    // 绑定语句
    auto bound_select_statement = binder.bind(*ast_select_statement);
    ASSERT_NE(bound_select_statement, nullptr);

    // 验证 BoundSelectStatement 的内容
    auto * select_stmt = dynamic_cast<const dreamdb::binder::bound::BoundSelectStatement *>(bound_select_statement.get());
    ASSERT_NE(select_stmt, nullptr);
    EXPECT_TRUE(select_stmt->has_limit());
    EXPECT_EQ(select_stmt->limit(), 20);
    EXPECT_TRUE(select_stmt->has_offset());
    EXPECT_EQ(select_stmt->offset(), 10);
}

/**
 * @brief 测试绑定 SELECT 带 GROUP BY 子句语句
 */
TEST_F(SelectBinderTest, BindSelectWithGroupBy)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 SELECT 语句
    std::vector<dreamdb::parser::ast::AstSelectItem> select_items;
    auto age_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("age", 1, 1);
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(
        std::move(age_column), std::nullopt
    ));

    // 创建 GROUP BY 子句：GROUP BY age
    std::vector<std::unique_ptr<dreamdb::parser::ast::AstExpression>> group_by;
    auto group_by_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("age", 1, 1);
    group_by.push_back(std::move(group_by_column));

    auto ast_select_statement = dreamdb::parser::ast::AstSelectStatement::create(
        "test_collection",
        std::move(select_items),
        nullptr,
        std::move(group_by),
        nullptr,
        {},
        std::nullopt,
        std::nullopt,
        1, 1
    );

    // 绑定语句
    auto bound_select_statement = binder.bind(*ast_select_statement);
    ASSERT_NE(bound_select_statement, nullptr);

    // 验证 BoundSelectStatement 的内容
    auto * select_stmt = dynamic_cast<const dreamdb::binder::bound::BoundSelectStatement *>(bound_select_statement.get());
    ASSERT_NE(select_stmt, nullptr);
    EXPECT_EQ(select_stmt->group_by_count(), 1);
}

/**
 * @brief 测试绑定 SELECT 带 HAVING 子句语句
 */
TEST_F(SelectBinderTest, BindSelectWithHaving)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 SELECT 语句
    std::vector<dreamdb::parser::ast::AstSelectItem> select_items;
    auto age_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("age", 1, 1);
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(
        std::move(age_column), std::nullopt
    ));

    // 创建 HAVING 子句：HAVING age > 18
    auto having_age_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("age", 1, 1);
    auto literal_eighteen = dreamdb::parser::ast::AstLiteralExpression::create_integer(18, 1, 1);
    auto having_expr = dreamdb::parser::ast::AstBinaryExpression::create(
        dreamdb::parser::ast::AstBinaryOperatorType::GreaterThan,
        std::move(having_age_column),
        std::move(literal_eighteen),
        1, 1
    );

    auto ast_select_statement = dreamdb::parser::ast::AstSelectStatement::create(
        "test_collection",
        std::move(select_items),
        nullptr,
        {},
        std::move(having_expr),
        {},
        std::nullopt,
        std::nullopt,
        1, 1
    );

    // 绑定语句
    auto bound_select_statement = binder.bind(*ast_select_statement);
    ASSERT_NE(bound_select_statement, nullptr);

    // 验证 BoundSelectStatement 的内容
    auto * select_stmt = dynamic_cast<const dreamdb::binder::bound::BoundSelectStatement *>(bound_select_statement.get());
    ASSERT_NE(select_stmt, nullptr);
    EXPECT_TRUE(select_stmt->has_having());
    ASSERT_NE(select_stmt->having(), nullptr);
}

/**
 * @brief 测试绑定 SELECT 完整语句（包含所有子句）
 */
TEST_F(SelectBinderTest, BindSelectComplete)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 SELECT 语句
    std::vector<dreamdb::parser::ast::AstSelectItem> select_items;
    auto id_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(
        std::move(id_column), std::nullopt
    ));

    // 创建 WHERE 子句：id > 0
    auto where_id_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    auto literal_zero = dreamdb::parser::ast::AstLiteralExpression::create_integer(0, 1, 1);
    auto where_expr = dreamdb::parser::ast::AstBinaryExpression::create(
        dreamdb::parser::ast::AstBinaryOperatorType::GreaterThan,
        std::move(where_id_column),
        std::move(literal_zero),
        1, 1
    );

    // 创建 GROUP BY 子句：GROUP BY id
    std::vector<std::unique_ptr<dreamdb::parser::ast::AstExpression>> group_by;
    auto group_by_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    group_by.push_back(std::move(group_by_column));

    // 创建 HAVING 子句：HAVING id < 100
    auto having_id_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    auto literal_hundred = dreamdb::parser::ast::AstLiteralExpression::create_integer(100, 1, 1);
    auto having_expr = dreamdb::parser::ast::AstBinaryExpression::create(
        dreamdb::parser::ast::AstBinaryOperatorType::LessThan,
        std::move(having_id_column),
        std::move(literal_hundred),
        1, 1
    );

    // 创建 ORDER BY 子句：ORDER BY id DESC
    std::vector<dreamdb::parser::ast::AstOrderByItem> order_by_items;
    auto order_by_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    order_by_items.push_back(dreamdb::parser::ast::AstOrderByItem(
        std::move(order_by_column), dreamdb::common::Direction::DESC
    ));

    auto ast_select_statement = dreamdb::parser::ast::AstSelectStatement::create(
        "test_collection",
        std::move(select_items),
        std::move(where_expr),
        std::move(group_by),
        std::move(having_expr),
        std::move(order_by_items),
        std::make_optional<std::uint64_t>(10),  // LIMIT 10
        std::make_optional<std::uint64_t>(5),    // OFFSET 5
        1, 1
    );

    // 绑定语句
    auto bound_select_statement = binder.bind(*ast_select_statement);
    ASSERT_NE(bound_select_statement, nullptr);

    // 验证 BoundSelectStatement 的内容
    auto * select_stmt = dynamic_cast<const dreamdb::binder::bound::BoundSelectStatement *>(bound_select_statement.get());
    ASSERT_NE(select_stmt, nullptr);
    EXPECT_EQ(select_stmt->collection_id(), 99);
    EXPECT_EQ(select_stmt->select_item_count(), 1);
    EXPECT_TRUE(select_stmt->has_where());
    EXPECT_EQ(select_stmt->group_by_count(), 1);
    EXPECT_TRUE(select_stmt->has_having());
    EXPECT_EQ(select_stmt->order_by_count(), 1);
    EXPECT_TRUE(select_stmt->has_limit());
    EXPECT_EQ(select_stmt->limit(), 10);
    EXPECT_TRUE(select_stmt->has_offset());
    EXPECT_EQ(select_stmt->offset(), 5);
}

/**
 * @brief 测试集合不存在的情况
 */
TEST_F(SelectBinderTest, BindSelectNonexistentCollection)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 SELECT 语句（不存在的集合）
    std::vector<dreamdb::parser::ast::AstSelectItem> select_items;
    auto id_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(
        std::move(id_column), std::nullopt
    ));

    auto ast_select_statement = dreamdb::parser::ast::AstSelectStatement::create(
        "nonexistent_collection",
        std::move(select_items),
        nullptr,
        {},
        nullptr,
        {},
        std::nullopt,
        std::nullopt,
        1, 1
    );

    // 绑定语句应该抛出异常
    EXPECT_THROW(
        {
            auto bound_select_statement = binder.bind(*ast_select_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试未设置当前数据库的情况
 */
TEST_F(SelectBinderTest, BindSelectWithoutCurrentDatabase)
{
    // 不绑定 USE，直接尝试绑定 SELECT
    std::vector<dreamdb::parser::ast::AstSelectItem> select_items;
    auto id_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(
        std::move(id_column), std::nullopt
    ));

    auto ast_select_statement = dreamdb::parser::ast::AstSelectStatement::create(
        "test_collection",
        std::move(select_items),
        nullptr,
        {},
        nullptr,
        {},
        std::nullopt,
        std::nullopt,
        1, 1
    );

    // 绑定语句应该抛出异常
    EXPECT_THROW(
        {
            auto bound_select_statement = binder.bind(*ast_select_statement);
        },
        std::runtime_error
    );
}

/**
 * @brief 测试 SELECT 多个 ORDER BY 项
 */
TEST_F(SelectBinderTest, BindSelectWithMultipleOrderBy)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 SELECT 语句
    std::vector<dreamdb::parser::ast::AstSelectItem> select_items;
    auto id_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(
        std::move(id_column), std::nullopt
    ));

    // 创建 ORDER BY 子句：ORDER BY id ASC, name DESC
    std::vector<dreamdb::parser::ast::AstOrderByItem> order_by_items;
    auto order_by_id = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    order_by_items.push_back(dreamdb::parser::ast::AstOrderByItem(
        std::move(order_by_id), dreamdb::common::Direction::ASC
    ));
    auto order_by_name = dreamdb::parser::ast::AstColumnReferenceExpression::create("name", 1, 1);
    order_by_items.push_back(dreamdb::parser::ast::AstOrderByItem(
        std::move(order_by_name), dreamdb::common::Direction::DESC
    ));

    auto ast_select_statement = dreamdb::parser::ast::AstSelectStatement::create(
        "test_collection",
        std::move(select_items),
        nullptr,
        {},
        nullptr,
        std::move(order_by_items),
        std::nullopt,
        std::nullopt,
        1, 1
    );

    // 绑定语句
    auto bound_select_statement = binder.bind(*ast_select_statement);
    ASSERT_NE(bound_select_statement, nullptr);

    // 验证 BoundSelectStatement 的内容
    auto * select_stmt = dynamic_cast<const dreamdb::binder::bound::BoundSelectStatement *>(bound_select_statement.get());
    ASSERT_NE(select_stmt, nullptr);
    EXPECT_EQ(select_stmt->order_by_count(), 2);
    EXPECT_EQ(select_stmt->order_by_at(0).direction, dreamdb::common::Direction::ASC);
    EXPECT_EQ(select_stmt->order_by_at(1).direction, dreamdb::common::Direction::DESC);
}

/**
 * @brief 测试 SELECT 多个 GROUP BY 项
 */
TEST_F(SelectBinderTest, BindSelectWithMultipleGroupBy)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 SELECT 语句
    std::vector<dreamdb::parser::ast::AstSelectItem> select_items;
    auto id_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(
        std::move(id_column), std::nullopt
    ));

    // 创建 GROUP BY 子句：GROUP BY id, name
    std::vector<std::unique_ptr<dreamdb::parser::ast::AstExpression>> group_by;
    auto group_by_id = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    group_by.push_back(std::move(group_by_id));
    auto group_by_name = dreamdb::parser::ast::AstColumnReferenceExpression::create("name", 1, 1);
    group_by.push_back(std::move(group_by_name));

    auto ast_select_statement = dreamdb::parser::ast::AstSelectStatement::create(
        "test_collection",
        std::move(select_items),
        nullptr,
        std::move(group_by),
        nullptr,
        {},
        std::nullopt,
        std::nullopt,
        1, 1
    );

    // 绑定语句
    auto bound_select_statement = binder.bind(*ast_select_statement);
    ASSERT_NE(bound_select_statement, nullptr);

    // 验证 BoundSelectStatement 的内容
    auto * select_stmt = dynamic_cast<const dreamdb::binder::bound::BoundSelectStatement *>(bound_select_statement.get());
    ASSERT_NE(select_stmt, nullptr);
    EXPECT_EQ(select_stmt->group_by_count(), 2);
}

/**
 * @brief 测试 SELECT 带复杂 WHERE 子句
 */
TEST_F(SelectBinderTest, BindSelectWithComplexWhere)
{
    // 先绑定 USE，设置 Binder 上下文
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create("test_db", 1, 1);
    auto bound_use_statement = binder.bind(*ast_use_statement);
    ASSERT_NE(bound_use_statement, nullptr);

    // 创建 SELECT 语句
    std::vector<dreamdb::parser::ast::AstSelectItem> select_items;
    auto id_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    select_items.push_back(dreamdb::parser::ast::AstSelectExpressionItem(
        std::move(id_column), std::nullopt
    ));

    // 创建 WHERE 子句：id = 1 AND age > 18
    auto where_id_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("id", 1, 1);
    auto literal_one = dreamdb::parser::ast::AstLiteralExpression::create_integer(1, 1, 1);
    auto id_eq_one = dreamdb::parser::ast::AstBinaryExpression::create(
        dreamdb::parser::ast::AstBinaryOperatorType::Equal,
        std::move(where_id_column),
        std::move(literal_one),
        1, 1
    );

    auto where_age_column = dreamdb::parser::ast::AstColumnReferenceExpression::create("age", 1, 1);
    auto literal_eighteen = dreamdb::parser::ast::AstLiteralExpression::create_integer(18, 1, 1);
    auto age_gt_eighteen = dreamdb::parser::ast::AstBinaryExpression::create(
        dreamdb::parser::ast::AstBinaryOperatorType::GreaterThan,
        std::move(where_age_column),
        std::move(literal_eighteen),
        1, 1
    );

    auto where_expr = dreamdb::parser::ast::AstBinaryExpression::create(
        dreamdb::parser::ast::AstBinaryOperatorType::And,
        std::move(id_eq_one),
        std::move(age_gt_eighteen),
        1, 1
    );

    auto ast_select_statement = dreamdb::parser::ast::AstSelectStatement::create(
        "test_collection",
        std::move(select_items),
        std::move(where_expr),
        {},
        nullptr,
        {},
        std::nullopt,
        std::nullopt,
        1, 1
    );

    // 绑定语句
    auto bound_select_statement = binder.bind(*ast_select_statement);
    ASSERT_NE(bound_select_statement, nullptr);

    // 验证 BoundSelectStatement 的内容
    auto * select_stmt = dynamic_cast<const dreamdb::binder::bound::BoundSelectStatement *>(bound_select_statement.get());
    ASSERT_NE(select_stmt, nullptr);
    EXPECT_TRUE(select_stmt->has_where());
    ASSERT_NE(select_stmt->where(), nullptr);
}