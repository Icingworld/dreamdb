#pragma once

#include <memory>
#include <functional>
#include <cstdint>

#include <asio.hpp>

namespace dreamdb
{

class Session;

/**
 * @brief 请求处理器函数类型
 * @param sql SQL 语句
 * @return pair<是否成功, 结果或错误信息>
 */
using RequestHandler = std::function<std::pair<bool, std::string>(const std::string & sql)>;

/**
 * @brief 简易 TCP 服务端
 * @details 基于 asio 实现的异步 TCP 服务器，用于接收 SQL 请求并返回结果
 */
class Server
{
public:
    /**
     * @brief 构造函数
     * @param port 监听端口
     */
    explicit Server(std::uint16_t port);

    Server(const Server &) = delete;

    Server(Server &&) = delete;

    Server & operator=(const Server &) = delete;

    Server & operator=(Server &&) = delete;

    ~Server();

public:
    /**
     * @brief 设置请求处理器
     * @param handler 处理 SQL 请求的回调函数
     */
    void set_request_handler(RequestHandler handler);

    /**
     * @brief 启动服务器（阻塞，直到调用 stop）
     */
    void run();

    /**
     * @brief 停止服务器
     */
    void stop();

private:
    /**
     * @brief 开始异步接受新连接
     */
    void do_accept();

private:
    asio::io_context io_context_;           // IO 上下文
    asio::ip::tcp::acceptor acceptor_;      // 接收器
    RequestHandler request_handler_;        // 请求处理器
};

} // namespace dreamdb
