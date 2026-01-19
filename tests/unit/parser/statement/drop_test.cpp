#include <gtest/gtest.h>
#include <dreamdb/parser/parser.h>
#include <dreamdb/parser/ast/statement/statement.h>
#include <dreamdb/parser/ast/debug/debug_printer.h>

class DropTest : public ::testing::Test
{
public:
    dreamdb::parser::ast::AstDebugPrinter printer;
};

TEST_F(DropTest, ParseDropDatabase)
{
    std::string sql = "DROP DATABASE test_db;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "DROP DATABASE test_db");
}

TEST_F(DropTest, ParseDropCollection)
{
    std::string sql = "DROP COLLECTION test_collection;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "DROP COLLECTION test_collection");
}

TEST_F(DropTest, ParseDropIndex)
{
    std::string sql = "DROP INDEX test_index ON test_collection;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "DROP INDEX test_index ON test_collection");
}

TEST_F(DropTest, ParseDropVIndex)
{
    std::string sql = "DROP VINDEX test_vindex ON test_collection;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "DROP VINDEX test_vindex ON test_collection");
}

TEST_F(DropTest, ParseDropDatabaseIfExists)
{
    std::string sql = "DROP DATABASE IF EXISTS test_db;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "DROP DATABASE IF EXISTS test_db");
}

TEST_F(DropTest, ParseDropCollectionIfExists)
{
    std::string sql = "DROP COLLECTION IF EXISTS test_collection;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "DROP COLLECTION IF EXISTS test_collection");
}

TEST_F(DropTest, ParseDropIndexIfExists)
{
    std::string sql = "DROP INDEX IF EXISTS test_index ON test_collection;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "DROP INDEX IF EXISTS test_index ON test_collection");
}

TEST_F(DropTest, ParseDropVIndexIfExists)
{
    std::string sql = "DROP VINDEX IF EXISTS test_vindex ON test_collection;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "DROP VINDEX IF EXISTS test_vindex ON test_collection");
}