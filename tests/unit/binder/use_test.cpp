#include <gtest/gtest.h>
#include <dreamdb/binder/binder.h>
#include <dreamdb/binder/bound/debug/debug_printer.h>
#include <dreamdb/parser/ast/statement/use.h>
#include <dreamdb/binder/bound/statement/use.h>
#include <optional>

#include "catalog_mock.h"

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
            .WillByDefault(::testing::Return(std::make_optional(12345)));

        ON_CALL(catalog_mock, resolve_database("test_db1"))
            .WillByDefault(::testing::Return(std::nullopt));
    }

public:
    CatalogMock catalog_mock;
    dreamdb::binder::Binder binder;
    dreamdb::binder::bound::BoundDebugPrinter printer;
};

TEST_F(UseBinderTest, BindUseStatement)
{
    // 构造 USE 语句语法树
    auto ast_use_statement = dreamdb::parser::ast::AstUseStatement::create(
        "test_db", 1, 1
    );

    // 绑定语句
    auto bound_statement = binder.bind(*ast_use_statement);

    ASSERT_NE(bound_statement, nullptr);

    EXPECT_EQ(printer.format(*bound_statement), "USE database_id:12345");
}
