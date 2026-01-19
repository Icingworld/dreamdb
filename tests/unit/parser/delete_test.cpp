#include <gtest/gtest.h>
#include <dreamdb/parser/parser.h>
#include <dreamdb/parser/ast/statement/statement.h>
#include <dreamdb/parser/ast/debug/debug_printer.h>

class DeleteTest : public ::testing::Test
{
public:
    dreamdb::parser::ast::AstDebugPrinter printer;
};

TEST_F(DeleteTest, ParseDelete)
{
    std::string sql = "DELETE FROM test_collection;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "DELETE FROM test_collection");
}

TEST_F(DeleteTest, ParseDeleteWhere)
{
    std::string sql = "DELETE FROM test_collection WHERE id = 1 AND name = 'test';";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "DELETE FROM test_collection WHERE ((id = 1) AND (name = 'test'))");
}
