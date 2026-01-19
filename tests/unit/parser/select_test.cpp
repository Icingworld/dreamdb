#include <gtest/gtest.h>
#include <dreamdb/parser/parser.h>
#include <dreamdb/parser/ast/statement/statement.h>
#include <dreamdb/parser/ast/debug/debug_printer.h>

class SelectTest : public ::testing::Test
{
public:
    dreamdb::parser::ast::AstDebugPrinter printer;
};

TEST_F(SelectTest, ParseSelectStar)
{
    std::string sql = "SELECT * FROM test_collection;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "SELECT * FROM test_collection");
}

TEST_F(SelectTest, ParseSelectColumn)
{
    std::string sql = "SELECT id FROM test_collection;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "SELECT id FROM test_collection");
}

TEST_F(SelectTest, ParseSelectMultipleColumns)
{
    std::string sql = "SELECT id, name, age FROM test_collection;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "SELECT id, name, age FROM test_collection");
}

TEST_F(SelectTest, ParseSelectWithAlias)
{
    std::string sql = "SELECT id AS user_id, name AS user_name FROM test_collection;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "SELECT id AS user_id, name AS user_name FROM test_collection");
}

TEST_F(SelectTest, ParseSelectWhere)
{
    std::string sql = "SELECT * FROM test_collection WHERE id = 1;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "SELECT * FROM test_collection WHERE (id = 1)");
}

TEST_F(SelectTest, ParseSelectWhereComplex)
{
    std::string sql = "SELECT * FROM test_collection WHERE id > 10 AND name = 'test';";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "SELECT * FROM test_collection WHERE ((id > 10) AND (name = 'test'))");
}

TEST_F(SelectTest, ParseSelectGroupBy)
{
    std::string sql = "SELECT category FROM test_collection GROUP BY category;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "SELECT category FROM test_collection GROUP BY category");
}

TEST_F(SelectTest, ParseSelectGroupByMultiple)
{
    std::string sql = "SELECT category, status FROM test_collection GROUP BY category, status;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "SELECT category, status FROM test_collection GROUP BY category, status");
}

TEST_F(SelectTest, ParseSelectHaving)
{
    std::string sql = "SELECT category FROM test_collection GROUP BY category HAVING id > 10;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "SELECT category FROM test_collection GROUP BY category HAVING (id > 10)");
}

TEST_F(SelectTest, ParseSelectOrderBy)
{
    std::string sql = "SELECT * FROM test_collection ORDER BY id;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "SELECT * FROM test_collection ORDER BY id ASC");
}

TEST_F(SelectTest, ParseSelectOrderByDesc)
{
    std::string sql = "SELECT * FROM test_collection ORDER BY id DESC;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "SELECT * FROM test_collection ORDER BY id DESC");
}

TEST_F(SelectTest, ParseSelectOrderByMultiple)
{
    std::string sql = "SELECT * FROM test_collection ORDER BY category ASC, name DESC;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "SELECT * FROM test_collection ORDER BY category ASC, name DESC");
}

TEST_F(SelectTest, ParseSelectLimit)
{
    std::string sql = "SELECT * FROM test_collection LIMIT 10;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "SELECT * FROM test_collection LIMIT 10");
}

TEST_F(SelectTest, ParseSelectOffset)
{
    std::string sql = "SELECT * FROM test_collection OFFSET 20;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "SELECT * FROM test_collection OFFSET 20");
}

TEST_F(SelectTest, ParseSelectLimitOffset)
{
    std::string sql = "SELECT * FROM test_collection LIMIT 10 OFFSET 20;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "SELECT * FROM test_collection LIMIT 10 OFFSET 20");
}

TEST_F(SelectTest, ParseSelectComplex)
{
    std::string sql = "SELECT category, name AS total FROM test_collection WHERE id > 0 GROUP BY category HAVING id > 10 ORDER BY total DESC LIMIT 5;";
    dreamdb::parser::Parser parser(sql);
    auto statement = parser.parse();

    EXPECT_EQ(printer.format(*statement), "SELECT category, name AS total FROM test_collection WHERE (id > 0) GROUP BY category HAVING (id > 10) ORDER BY total DESC LIMIT 5");
}
