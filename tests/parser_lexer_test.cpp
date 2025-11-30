#include <dreamdb/parser/lexer.h>
#include <iostream>
#include <vector>

using namespace dreamdb;

void test_simple_select()
{
    std::cout << "Test 1: Simple SELECT statement" << std::endl;
    
    Lexer lexer("SELECT * FROM users");
    
    std::vector<Token> tokens;
    while (lexer.has_more()) {
        Token token = lexer.next_token();
        if (token.get_type() == TokenType::EOF_TOKEN) {
            break;
        }
        tokens.push_back(token);
        std::cout << "  " << token.to_string() << std::endl;
    }
    
    std::cout << "  Found " << tokens.size() << " tokens" << std::endl;
    std::cout << std::endl;
}

void test_select_with_where()
{
    std::cout << "Test 2: SELECT with WHERE condition" << std::endl;
    
    Lexer lexer("SELECT name, age FROM users WHERE id = 1");
    
    std::vector<Token> tokens;
    while (lexer.has_more()) {
        Token token = lexer.next_token();
        if (token.get_type() == TokenType::EOF_TOKEN) {
            break;
        }
        tokens.push_back(token);
        std::cout << "  " << token.to_string() << std::endl;
    }
    
    std::cout << "  Found " << tokens.size() << " tokens" << std::endl;
    std::cout << std::endl;
}

void test_insert()
{
    std::cout << "Test 3: INSERT statement" << std::endl;
    
    Lexer lexer("INSERT INTO users (name, age) VALUES ('Alice', 25)");
    
    std::vector<Token> tokens;
    while (lexer.has_more()) {
        Token token = lexer.next_token();
        if (token.get_type() == TokenType::EOF_TOKEN) {
            break;
        }
        tokens.push_back(token);
        std::cout << "  " << token.to_string() << std::endl;
    }
    
    std::cout << "  Found " << tokens.size() << " tokens" << std::endl;
    std::cout << std::endl;
}

int main()
{
    std::cout << "=== Lexer Test ===" << std::endl;
    std::cout << std::endl;

    try {
        test_simple_select();
        test_select_with_where();
        test_insert();

        std::cout << "All tests passed!" << std::endl;
        return 0;
    }
    catch (const std::exception & e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

