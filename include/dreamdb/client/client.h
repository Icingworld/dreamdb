#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <memory>

#include <asio.hpp>

#include "dreamdb/protocol/codec.h"
#include "dreamdb/protocol/serializer.h"
#include "dreamdb/protocol/message.pb.h"

namespace dreamdb
{

/**
 * @brief 简易 TCP 客户端
 * @details 用于连接 dreamdb::Server，发送 SQL 请求并接收结果
 */
class Client
{
public:
    Client();

    Client(const Client &) = delete;

    Client(Client &&) = delete;

    Client & operator=(const Client &) = delete;

    Client & operator=(Client &&) = delete;

    ~Client();

public:
    /**
     * @brief 连接到服务器
     * @param host 服务器地址
     * @param port 服务器端口
     * @throw std::runtime_error 如果连接失败
     */
    void connect(const std::string & host, std::uint16_t port);

    /**
     * @brief 断开连接
     */
    void disconnect();

    /**
     * @brief 检查是否已连接
     * @return 如果已连接返回 true
     */
    bool is_connected() const noexcept;

    /**
     * @brief 执行 SQL 语句
     * @param sql SQL 语句
     * @return 响应对象
     * @throw std::runtime_error 如果发送或接收失败
     */
    Response execute(const std::string & sql);

private:
    /**
     * @brief 发送请求并接收响应
     * @param request 请求对象
     * @return 响应对象
     * @throw std::runtime_error 如果发送或接收失败
     */
    Response send_request(const Request & request);

    /**
     * @brief 发送数据
     * @param data 要发送的数据
     */
    void send(const std::vector<std::uint8_t> & data);

    /**
     * @brief 接收一个完整的响应
     * @return 响应对象
     */
    Response receive_response();

private:
    asio::io_context io_context_;               // IO 上下文
    std::unique_ptr<asio::ip::tcp::socket> socket_;  // Socket
    Serializer serializer_;                     // 序列化器
    std::vector<std::uint8_t> recv_buffer_;     // 接收缓冲区
    std::uint64_t next_request_id_;             // 下一个请求 ID
    bool connected_;                            // 是否已连接
};

} // namespace dreamdb
