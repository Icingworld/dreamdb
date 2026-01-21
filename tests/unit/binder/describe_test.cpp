#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <dreamdb/binder/binder.h>
#include <dreamdb/binder/bound/debug/debug_printer.h>
#include <dreamdb/binder/bound/statement/statement.h>
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
    }

public:
    CatalogMock catalog_mock;
    dreamdb::binder::Binder binder;
    dreamdb::binder::bound::BoundDebugPrinter printer;
};

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

    EXPECT_EQ(printer.format(*bound_describe_statement), "DESCRIBE collection_id:99");
}
