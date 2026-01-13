/**
 * @file server_demo.cpp
 * @brief 简易服务端示例
 * 运行：
 *   ./server_demo
 */

#include <iostream>
#include <string>
#include <memory>

#include "dreamdb/server/server.h"
#include "dreamdb/executor/executor.h"
#include "dreamdb/parser/parser.h"
#include "dreamdb/binder/binder.h"
#include "dreamdb/dispatcher/dispatcher.h"
#include "dreamdb/schema/database_manager.h"

int main()
{
    try {
        // 创建数据库管理器
        auto db_manager = std::make_unique<dreamdb::DatabaseManager>();
        
        // 创建执行器
        dreamdb::Executor executor(std::move(db_manager));
        
        // 创建调度器（Dispatcher 需要 Executor 和 Catalog 引用）
        dreamdb::Dispatcher dispatcher(executor, executor.get_catalog());
        
        // 创建服务器，监听 9527 端口
        dreamdb::Server server(9527);

        // 设置请求处理器
        server.set_request_handler([&executor, &dispatcher](const std::string & sql) -> std::pair<bool, std::string> {
            std::cout << "[Handler] Processing SQL: " << sql << std::endl;

            try {
                // 1. 语法分析：SQL -> AST
                dreamdb::Parser parser(sql);
                auto ast = parser.parse();
                
                if (!ast) {
                    return {false, "Failed to parse SQL"};
                }
                
                // 2. 绑定：AST -> BoundStatement
                // 获取当前数据库名称和 Catalog（从 Executor 获取）
                std::string current_database = executor.get_current_database_name();
                
                // 创建 Binder（使用 Catalog 引用，不复制）
                dreamdb::Binder binder(executor.get_catalog(), current_database);
                auto bound_statement = binder.bind(*ast);
                
                if (!bound_statement) {
                    return {false, "Failed to bind statement"};
                }
                
                // 3. 调度：BoundStatement -> Dispatcher -> Executor
                // Dispatcher 会根据语句类型决定执行路径：
                // - SELECT/UPDATE/DELETE: LogicalPlanner -> PhysicalPlanner -> Executor
                // - 其他: 直接 -> Executor
                auto result = dispatcher.dispatch(*bound_statement);
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
