#include <gtest/gtest.h>
#include <dreamdb/parser/parser.h>
#include <dreamdb/parser/ast/statement/statement.h>
#include <dreamdb/parser/ast/debug/debug_printer.h>

class InsertTest : public ::testing::Test
{
public:
    dreamdb::parser::ast::AstDebugPrinter printer;
};

TEST_F(InsertTest, ParseInsert)
{
    std::string sql = "INSERT INTO test_collection (id, name) VALUES (1, 'test');";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "INSERT INTO test_collection (id, name) VALUES (1, 'test')");
}

TEST_F(InsertTest, ParseInsertWithoutColumns)
{
    std::string sql = "INSERT INTO test_collection VALUES (1, 'test', 20);";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "INSERT INTO test_collection VALUES (1, 'test', 20)");
}