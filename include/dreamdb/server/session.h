#pragma once

#include <memory>
#include <vector>
#include <cstdint>
#include <functional>

#include <asio.hpp>

#include "dreamdb/protocol/codec.h"
#include "dreamdb/protocol/serializer.h"

namespace dreamdb
{

/**
 * @brief 请求处理器函数类型
 */
using RequestHandler = std::function<std::pair<bool, std::string>(const std::string & sql)>;

/**
 * @brief 连接会话
 * @details 处理单个客户端连接的读写操作
 */
class Session : public std::enable_shared_from_this<Session>
{
public:
    /**
     * @brief 构造函数
     * @param socket 已连接的 socket
     * @param handler 请求处理器
     */
    Session(asio::ip::tcp::socket socket, RequestHandler handler);

    Session(const Session &) = delete;

    Session(Session &&) = delete;

    Session & operator=(const Session &) = delete;

    Session & operator=(Session &&) = delete;

    ~Session() = default;

public:
    /**
     * @brief 启动会话，开始异步读取
     */
    void start();

private:
    /**
     * @brief 异步读取数据
     */
    void do_read();

    /**
     * @brief 处理收到的数据
     */
    void process_buffer();

    /**
     * @brief 发送响应
     * @param response 响应对象
     */
    void send_response(const Response & response);

private:
    asio::ip::tcp::socket socket_;
    RequestHandler request_handler_;
    Serializer serializer_;

    std::vector<std::uint8_t> recv_buffer_;             // 接收缓冲区
    std::array<std::uint8_t, 8192> temp_buffer_;        // 临时读取缓冲区
};

} // namespace dreamdb

