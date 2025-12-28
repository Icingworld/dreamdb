/**
 * @file auto_client.cpp
 * @brief 自动执行 SQL 文件的客户端
 * 运行：
 *   ./auto_client [sql_file]
 */

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "dreamdb/client/client.h"

namespace fs = std::filesystem;

/**
 * @brief 解析 SQL 文件，提取 SQL 语句
 * @param content 文件内容
 * @return SQL 语句列表
 */
std::vector<std::string> parse_sql_file(const std::string & content)
{
    std::vector<std::string> statements;
    std::ostringstream current_stmt;
    bool in_single_quote = false;
    bool in_double_quote = false;
    bool in_line_comment = false;
    bool in_block_comment = false;
    
    for (std::size_t i = 0; i < content.length(); ++i) {
        char c = content[i];
        char next = (i + 1 < content.length()) ? content[i + 1] : '\0';
        
        // 处理单引号字符串
        if (c == '\'' && !in_double_quote && !in_line_comment && !in_block_comment) {
            in_single_quote = !in_single_quote;
            current_stmt << c;
            continue;
        }
        
        // 处理双引号字符串
        if (c == '"' && !in_single_quote && !in_line_comment && !in_block_comment) {
            in_double_quote = !in_double_quote;
            current_stmt << c;
            continue;
        }
        
        // 如果在字符串中，直接添加字符
        if (in_single_quote || in_double_quote) {
            current_stmt << c;
            continue;
        }
        
        // 处理行注释 --
        if (c == '-' && next == '-' && !in_line_comment && !in_block_comment) {
            in_line_comment = true;
            ++i; // 跳过下一个 '-'
            continue;
        }
        
        // 处理块注释开始 /*
        if (c == '/' && next == '*' && !in_line_comment && !in_block_comment) {
            in_block_comment = true;
            ++i; // 跳过 '*'
            continue;
        }
        
        // 处理块注释结束 */
        if (c == '*' && next == '/' && in_block_comment) {
            in_block_comment = false;
            ++i; // 跳过 '/'
            continue;
        }
        
        // 如果在注释中，跳过字符
        if (in_line_comment || in_block_comment) {
            if (c == '\n') {
                in_line_comment = false;
            }
            continue;
        }
        
        // 处理分号（语句结束符）
        if (c == ';') {
            std::string stmt = current_stmt.str();
            // 去除首尾空白
            std::size_t start = stmt.find_first_not_of(" \t\n\r");
            if (start != std::string::npos) {
                std::size_t end = stmt.find_last_not_of(" \t\n\r");
                stmt = stmt.substr(start, end - start + 1);
                if (!stmt.empty()) {
                    statements.push_back(stmt);
                }
            }
            current_stmt.str("");
            current_stmt.clear();
            continue;
        }
        
        // 添加字符到当前语句
        current_stmt << c;
    }
    
    // 处理最后一个语句（如果没有分号结尾）
    std::string stmt = current_stmt.str();
    std::size_t start = stmt.find_first_not_of(" \t\n\r");
    if (start != std::string::npos) {
        std::size_t end = stmt.find_last_not_of(" \t\n\r");
        stmt = stmt.substr(start, end - start + 1);
        if (!stmt.empty()) {
            statements.push_back(stmt);
        }
    }
    
    return statements;
}

/**
 * @brief 读取文件内容
 * @param filepath 文件路径
 * @return 文件内容
 */
std::string read_file(const std::string & filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filepath);
    }
    
    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
}

int main(int argc, char * argv[])
{
    try {
        // 确定 SQL 文件路径
        std::string sql_file;
        if (argc > 1) {
            sql_file = argv[1];
        } else {
            // 默认使用当前工作目录下的 test.sql
            sql_file = "test.sql";
        }
        
        // 转换为绝对路径
        fs::path sql_path(sql_file);
        if (!sql_path.is_absolute()) {
            sql_path = fs::absolute(sql_path);
        }
        sql_file = sql_path.string();
        
        std::cout << "Reading SQL file: " << sql_file << std::endl;
        
        // 读取 SQL 文件
        std::string content = read_file(sql_file);
        
        // 解析 SQL 语句
        std::vector<std::string> statements = parse_sql_file(content);
        
        if (statements.empty()) {
            std::cerr << "No SQL statements found in file." << std::endl;
            return 1;
        }
        
        std::cout << "Found " << statements.size() << " SQL statement(s)." << std::endl;
        std::cout << std::endl;
        
        // 创建客户端并连接
        dreamdb::Client client;
        client.connect("127.0.0.1", 9527);
        std::cout << "Connected to server." << std::endl;
        std::cout << std::endl;
        
        // 执行每条 SQL 语句
        for (std::size_t i = 0; i < statements.size(); ++i) {
            const std::string & sql = statements[i];
            
            std::cout << "[" << (i + 1) << "/" << statements.size() << "] Executing:" << std::endl;
            std::cout << sql << std::endl;
            std::cout << std::endl;
            
            try {
                auto response = client.execute(sql);
                
                std::cout << "Response #" << response.request_id() << ":" << std::endl;
                if (response.success()) {
                    std::cout << "Success: " << response.result() << std::endl;
                } else {
                    std::cout << "Error: " << response.error_message() << std::endl;
                }
                std::cout << std::endl;
                
            } catch (const std::exception & e) {
                std::cerr << "Execute error: " << e.what() << std::endl;
                std::cerr << std::endl;
            }
        }
        
        // 断开连接
        client.disconnect();
        std::cout << "Disconnected." << std::endl;
        
    } catch (const std::exception & e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
