#include <gtest/gtest.h>
#include <dreamdb/parser/parser.h>
#include <dreamdb/parser/ast/statement/statement.h>
#include <dreamdb/parser/ast/debug/debug_printer.h>

class ShowTest : public ::testing::Test
{
public:
    dreamdb::parser::ast::AstDebugPrinter printer;
};

TEST_F(ShowTest, ParseShowDatabases)
{
    std::string sql = "SHOW DATABASES;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "SHOW DATABASES");
}

TEST_F(ShowTest, ParseShowCollections)
{
    std::string sql = "SHOW COLLECTIONS;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "SHOW COLLECTIONS");
}

TEST_F(ShowTest, ParseShowCollectionsFromDatabase)
{
    std::string sql = "SHOW COLLECTIONS FROM test_db;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "SHOW COLLECTIONS FROM test_db");
}

TEST_F(ShowTest, ParseShowIndexes)
{
    std::string sql = "SHOW INDEXES FROM test_collection;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "SHOW INDEXES FROM test_collection");
}

TEST_F(ShowTest, ParseShowIndexesFromDatabase)
{
    std::string sql = "SHOW INDEXES FROM test_collection FROM test_db;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "SHOW INDEXES FROM test_collection FROM test_db");
}

TEST_F(ShowTest, ParseShowVIndexes)
{
    std::string sql = "SHOW VINDEXES FROM test_collection;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "SHOW VINDEXES FROM test_collection");
}

TEST_F(ShowTest, ParseShowVIndexesFromDatabase)
{
    std::string sql = "SHOW VINDEXES FROM test_collection FROM test_db;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "SHOW VINDEXES FROM test_collection FROM test_db");
}
