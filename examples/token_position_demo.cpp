#include <dreamdb/parser/lexer.h>
#include <iostream>

using namespace dreamdb;

void demonstrate_error_reporting()
{
    std::cout << "=== 演示：错误报告中的行号列号 ===" << std::endl;
    std::cout << std::endl;

    // 模拟一个错误的 SQL
    std::string sql = "SELECT name FROM users WHERE id = 'hello'";
    
    std::cout << "SQL: " << sql << std::endl;
    std::cout << std::endl;

    Lexer lexer(sql);
    
    // 找到 'hello' 这个 Token
    Token token;
    while (lexer.has_more()) {
        token = lexer.next_token();
        if (token.get_type() == TokenType::STRING_LITERAL && token.get_value() == "hello") {
            break;
        }
    }

    // 模拟类型检查错误
    std::cout << "错误：类型不匹配" << std::endl;
    std::cout << "  期望类型：INT64" << std::endl;
    std::cout << "  实际类型：STRING" << std::endl;
    std::cout << "  位置：第 " << token.get_line() << " 行，第 " << token.get_column() << " 列" << std::endl;
    std::cout << std::endl;
    
    // 可视化错误位置
    std::cout << "SQL: " << sql << std::endl;
    std::cout << "     ";
    for (std::size_t i = 0; i < token.get_column() - 1; ++i) {
        std::cout << " ";
    }
    std::cout << "^";
    for (std::size_t i = 1; i < token.get_value().length(); ++i) {
        std::cout << "^";
    }
    std::cout << " (这里出错了)" << std::endl;
    std::cout << std::endl;
}

void demonstrate_multiline_error()
{
    std::cout << "=== 演示：多行 SQL 的错误定位 ===" << std::endl;
    std::cout << std::endl;

    // 多行 SQL
    std::string sql = 
        "SELECT name, age\n"
        "FROM users\n"
        "WHERE id = 'hello'\n"
        "  AND status = 'active'";
    
    std::cout << "SQL:" << std::endl;
    std::cout << sql << std::endl;
    std::cout << std::endl;

    Lexer lexer(sql);
    
    // 找到 'hello' 这个 Token
    Token token;
    while (lexer.has_more()) {
        token = lexer.next_token();
        if (token.get_type() == TokenType::STRING_LITERAL && token.get_value() == "hello") {
            break;
        }
    }

    std::cout << "错误：类型不匹配" << std::endl;
    std::cout << "  位置：第 " << token.get_line() << " 行，第 " << token.get_column() << " 列" << std::endl;
    std::cout << std::endl;
    
    // 显示错误所在的行
    std::size_t line_start = 0;
    std::size_t line_num = 1;
    for (std::size_t i = 0; i < sql.length(); ++i) {
        if (line_num == token.get_line()) {
            line_start = i;
            break;
        }
        if (sql[i] == '\n') {
            line_num++;
        }
    }
    
    std::size_t line_end = line_start;
    while (line_end < sql.length() && sql[line_end] != '\n') {
        line_end++;
    }
    
    std::string error_line = sql.substr(line_start, line_end - line_start);
    std::cout << "第 " << token.get_line() << " 行: " << error_line << std::endl;
    std::cout << "      ";
    for (std::size_t i = 0; i < token.get_column() - 1; ++i) {
        std::cout << " ";
    }
    std::cout << "^";
    for (std::size_t i = 1; i < token.get_value().length(); ++i) {
        std::cout << "^";
    }
    std::cout << " (这里出错了)" << std::endl;
    std::cout << std::endl;
}

int main()
{
    demonstrate_error_reporting();
    demonstrate_multiline_error();
    return 0;
}

