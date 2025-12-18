/**
 * @file server_demo.cpp
 * @brief 简易服务端示例
 * 运行：
 *   ./server_demo
 */

#include <iostream>
#include <string>

#include "dreamdb/server/server.h"
#include "dreamdb/executor/executor.h"
#include "dreamdb/parser/parser.h"
#include "dreamdb/schema/database_manager.h"

dreamdb::DatabaseManager db_manager;
dreamdb::Executor executor(db_manager);

int main()
{
    try {
        // 创建服务器，监听 9527 端口
        dreamdb::Server server(9527);

        // 设置请求处理器
        server.set_request_handler([](const std::string & sql) -> std::pair<bool, std::string> {
            std::cout << "[Handler] Processing SQL: " << sql << std::endl;

            try {                
                // 1. 语法分析
                dreamdb::Parser parser(sql);
                auto ast = parser.parse();

                // 2. 执行
                auto result = executor.execute(*ast);
                return {result.is_success(), result.get_message()};
            } catch (const std::exception & e) {
                return {false, e.what()};
            }
        });

        std::cout << "Server is running on port 9527..." << std::endl;
        std::cout << "Press Ctrl+C to stop." << std::endl;

        // 阻塞运行
        server.run();

    } catch (const std::exception & e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

