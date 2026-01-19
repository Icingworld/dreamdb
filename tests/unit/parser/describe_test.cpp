#include <gtest/gtest.h>
#include <dreamdb/parser/parser.h>
#include <dreamdb/parser/ast/statement/statement.h>
#include <dreamdb/parser/ast/debug/debug_printer.h>

class DescribeTest : public ::testing::Test
{
public:
    dreamdb::parser::ast::AstDebugPrinter printer;
};

TEST_F(DescribeTest, ParseDescribe)
{
    std::string sql = "DESCRIBE test_collection;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "DESCRIBE test_collection");
}

TEST_F(DescribeTest, ParseDesc)
{
    std::string sql = "DESC test_collection;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "DESCRIBE test_collection");
}
