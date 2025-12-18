/**
 * @file client_demo.cpp
 * @brief 简易客户端示例
 * 运行：
 *   ./client_demo
 */

#include <iostream>
#include <string>

#include "dreamdb/client/client.h"

int main()
{
    try {
        // 创建客户端
        dreamdb::Client client;

        // 连接到服务器
        client.connect("127.0.0.1", 9527);
        std::cout << "Connected to server." << std::endl;

        // 交互式发送 SQL
        std::string line;
        while (true) {
            std::cout << "sql> ";
            if (!std::getline(std::cin, line)) {
                break;
            }

            if (line.empty()) {
                continue;
            }

            if (line == "exit" || line == "quit") {
                break;
            }

            try {
                // 执行 SQL
                auto response = client.execute(line);

                // 输出结果
                std::cout << "Response #" << response.request_id() << ":" << std::endl;
                if (response.success()) {
                    std::cout << "  Result: " << response.result() << std::endl;
                } else {
                    std::cout << "  Error: " << response.error_message() << std::endl;
                }
                std::cout << std::endl;

            } catch (const std::exception & e) {
                std::cerr << "Execute error: " << e.what() << std::endl;
            }
        }

        // 断开连接
        client.disconnect();
        std::cout << "Disconnected." << std::endl;

    } catch (const std::exception & e) {
        std::cerr << "Client error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
