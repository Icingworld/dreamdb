#include <gtest/gtest.h>
#include <dreamdb/parser/parser.h>
#include <dreamdb/parser/ast/statement/statement.h>
#include <dreamdb/parser/ast/debug/debug_printer.h>

class AlterTest : public ::testing::Test
{
public:
    dreamdb::parser::ast::AstDebugPrinter printer;
};

TEST_F(AlterTest, ParseAlterAddColumn)
{
    std::string sql = "ALTER COLLECTION test_collection ADD COLUMN new_column INT;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "ALTER COLLECTION test_collection ADD COLUMN new_column INT");
}

TEST_F(AlterTest, ParseAlterAddColumnWithModifiers)
{
    std::string sql = "ALTER COLLECTION test_collection ADD COLUMN new_column VARCHAR(255) NOT NULL DEFAULT 'test';";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "ALTER COLLECTION test_collection ADD COLUMN new_column VARCHAR(255) NOT NULL DEFAULT 'test'");
}

TEST_F(AlterTest, ParseAlterDropColumn)
{
    std::string sql = "ALTER COLLECTION test_collection DROP COLUMN old_column;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "ALTER COLLECTION test_collection DROP COLUMN old_column");
}

TEST_F(AlterTest, ParseAlterModifyColumn)
{
    std::string sql = "ALTER COLLECTION test_collection MODIFY COLUMN existing_column VARCHAR(100);";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "ALTER COLLECTION test_collection MODIFY COLUMN existing_column VARCHAR(100)");
}

TEST_F(AlterTest, ParseAlterModifyColumnWithModifiers)
{
    std::string sql = "ALTER COLLECTION test_collection MODIFY COLUMN existing_column INT NOT NULL PRIMARY KEY;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "ALTER COLLECTION test_collection MODIFY COLUMN existing_column INT NOT NULL PRIMARY KEY");
}

TEST_F(AlterTest, ParseAlterRenameColumn)
{
    std::string sql = "ALTER COLLECTION test_collection RENAME COLUMN old_name TO new_name;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "ALTER COLLECTION test_collection RENAME COLUMN old_name TO new_name");
}
