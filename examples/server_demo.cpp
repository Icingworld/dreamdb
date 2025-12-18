/**
 * @file server_demo.cpp
 * @brief 简易服务端示例
 * 运行：
 *   ./server_demo
 */

#include <iostream>
#include <string>

#include "dreamdb/server/server.h"

int main()
{
    try {
        // 创建服务器，监听 9527 端口
        dreamdb::Server server(9527);

        // 设置请求处理器
        server.set_request_handler([](const std::string & sql) -> std::pair<bool, std::string> {
            std::cout << "[Handler] Processing SQL: " << sql << std::endl;

            // 这里是简单的 echo 逻辑，实际应该调用 parser -> planner -> executor
            // 例如：
            //   auto ast = parser.parse(sql);
            //   auto plan = planner.plan(ast);
            //   auto result = executor.execute(plan);
            //   return {true, result.to_json()};

            // 简单演示：返回收到的 SQL
            if (sql.empty()) {
                return {false, "Empty SQL statement"};
            }

            // 模拟处理结果
            std::string result = "Executed: " + sql;
            return {true, result};
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

