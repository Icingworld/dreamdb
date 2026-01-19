#include <gtest/gtest.h>
#include <dreamdb/parser/parser.h>
#include <dreamdb/parser/ast/statement/statement.h>
#include <dreamdb/parser/ast/debug/debug_printer.h>

class CreateTest : public ::testing::Test
{
public:
    dreamdb::parser::ast::AstDebugPrinter printer;
};

TEST_F(CreateTest, ParseCreateDatabase)
{
    std::string sql = "CREATE DATABASE test_db;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "CREATE DATABASE test_db");
}

TEST_F(CreateTest, ParseCreateDatabaseIfNotExists)
{
    std::string sql = "CREATE DATABASE IF NOT EXISTS test_db;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "CREATE DATABASE IF NOT EXISTS test_db");
}

TEST_F(CreateTest, ParseCreateCollection)
{
    std::string sql = "CREATE COLLECTION test_collection (id INT PRIMARY KEY NOT NULL, name VARCHAR(255), age INT AUTO_INCREMENT);";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "CREATE COLLECTION test_collection (id INT PRIMARY KEY NOT NULL, name VARCHAR(255), age INT AUTO_INCREMENT)");
}

TEST_F(CreateTest, ParseCreateCollectionIfNotExists)
{
    std::string sql = "CREATE COLLECTION IF NOT EXISTS test_collection (id INT, name VARCHAR(255));";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "CREATE COLLECTION IF NOT EXISTS test_collection (id INT, name VARCHAR(255))");
}

TEST_F(CreateTest, ParseCreateIndex)
{
    std::string sql = "CREATE INDEX test_index ON test_collection (id, name);";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "CREATE INDEX test_index ON test_collection (id, name)");
}

TEST_F(CreateTest, ParseCreateIndexIfNotExists)
{
    std::string sql = "CREATE INDEX IF NOT EXISTS test_index ON test_collection (id);";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "CREATE INDEX IF NOT EXISTS test_index ON test_collection (id)");
}

TEST_F(CreateTest, ParseCreateIndexWithType)
{
    std::string sql = "CREATE INDEX test_index ON test_collection (id) USING HASH;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "CREATE INDEX test_index ON test_collection (id) USING HASH");
}

TEST_F(CreateTest, ParseCreateVIndex)
{
    std::string sql = "CREATE VINDEX test_vindex ON test_collection (id);";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "CREATE VINDEX test_vindex ON test_collection.id");
}

TEST_F(CreateTest, ParseCreateVIndexIfNotExists)
{
    std::string sql = "CREATE VINDEX IF NOT EXISTS test_vindex ON test_collection (id);";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "CREATE VINDEX IF NOT EXISTS test_vindex ON test_collection.id");
}

TEST_F(CreateTest, ParseCreateVIndexWithType)
{
    std::string sql = "CREATE VINDEX test_vindex ON test_collection (id) USING HASH;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "CREATE VINDEX test_vindex ON test_collection.id USING HASH");
}

TEST_F(CreateTest, ParseCreateVIndexWithOptions)
{
    std::string sql = "CREATE VINDEX test_vindex ON test_collection (id) USING HNSW WITH (m = 16, ef_construction = 100);";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "CREATE VINDEX test_vindex ON test_collection.id USING HNSW WITH (m = 16, ef_construction = 100)");
}
