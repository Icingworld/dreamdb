#include <gtest/gtest.h>
#include <dreamdb/parser/parser.h>
#include <dreamdb/parser/ast/statement/statement.h>
#include <dreamdb/parser/ast/debug/debug_printer.h>

class UpdateTest : public ::testing::Test
{
public:
    dreamdb::parser::ast::AstDebugPrinter printer;
};

TEST_F(UpdateTest, ParseUpdate)
{
    std::string sql = "UPDATE test_collection SET name = 'test';";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "UPDATE test_collection SET name = 'test'");
}

TEST_F(UpdateTest, ParseUpdateWhere)
{
    std::string sql = "UPDATE test_collection SET name = 'test' WHERE id = 1;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "UPDATE test_collection SET name = 'test' WHERE (id = 1)");
}

TEST_F(UpdateTest, ParseUpdateMultiple)
{
    std::string sql = "UPDATE test_collection SET name = 'test', age = 20;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "UPDATE test_collection SET name = 'test', age = 20");
}

TEST_F(UpdateTest, ParseUpdateMultipleWhere)
{
    std::string sql = "UPDATE test_collection SET name = 'test', age = 20 WHERE id = 1;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "UPDATE test_collection SET name = 'test', age = 20 WHERE (id = 1)");
}