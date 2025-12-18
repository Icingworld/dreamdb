#include "dreamdb/client/client.h"

#include <stdexcept>
#include <array>

namespace dreamdb
{

Client::Client()
    : io_context_()
    , socket_(nullptr)
    , serializer_()
    , recv_buffer_()
    , next_request_id_(1)
    , connected_(false)
{
}

Client::~Client()
{
    disconnect();
}

void Client::connect(const std::string & host, std::uint16_t port)
{
    if (connected_) {
        throw std::runtime_error("Client::connect: already connected");
    }

    try {
        socket_ = std::make_unique<asio::ip::tcp::socket>(io_context_);

        asio::ip::tcp::resolver resolver(io_context_);
        auto endpoints = resolver.resolve(host, std::to_string(port));
        asio::connect(*socket_, endpoints);

        connected_ = true;
        recv_buffer_.clear();

    } catch (const std::exception & e) {
        socket_.reset();
        throw std::runtime_error(std::string("Client::connect: ") + e.what());
    }
}

void Client::disconnect()
{
    if (socket_ && connected_) {
        try {
            socket_->shutdown(asio::ip::tcp::socket::shutdown_both);
        } catch (...) {
            // 忽略 shutdown 错误（连接可能已经关闭）
        }
        try {
            socket_->close();
        } catch (...) {
            // 忽略 close 错误
        }
    }
    socket_.reset();
    connected_ = false;
    recv_buffer_.clear();
}

bool Client::is_connected() const noexcept
{
    return connected_ && socket_ && socket_->is_open();
}

Response Client::execute(const std::string & sql)
{
    // 创建请求
    Request request;
    request.set_request_id(next_request_id_++);
    request.set_sql(sql);
    request.set_version(1);

    return send_request(request);
}

Response Client::send_request(const Request & request)
{
    if (!is_connected()) {
        throw std::runtime_error("Client::send_request: not connected");
    }

    // 序列化 + 编码
    auto serialized = serializer_.serialize_request(request);
    auto encoded = Codec::encode(serialized);

    // 发送
    send(encoded);

    // 接收响应
    return receive_response();
}

void Client::send(const std::vector<std::uint8_t> & data)
{
    if (!socket_) {
        throw std::runtime_error("Client::send: socket not initialized");
    }

    std::size_t total_sent = 0;
    while (total_sent < data.size()) {
        std::size_t sent = socket_->write_some(
            asio::buffer(data.data() + total_sent, data.size() - total_sent)
        );
        total_sent += sent;
    }
}

Response Client::receive_response()
{
    if (!socket_) {
        throw std::runtime_error("Client::receive_response: socket not initialized");
    }

    std::array<std::uint8_t, 4096> temp_buffer;
    std::vector<std::uint8_t> body;

    // 循环读取直到收到完整响应
    while (!Codec::try_decode_one(recv_buffer_, body)) {
        std::size_t n = socket_->read_some(asio::buffer(temp_buffer));
        if (n == 0) {
            throw std::runtime_error("Client::receive_response: connection closed");
        }
        recv_buffer_.insert(recv_buffer_.end(), temp_buffer.begin(), temp_buffer.begin() + n);
    }

    // 反序列化响应
    return serializer_.deserialize_response(body);
}

} // namespace dreamdb
